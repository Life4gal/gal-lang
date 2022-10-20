#include <gsl/utils/memory_model.hpp>

#include <gsl/utils/assert.hpp>
#include <gsl/boost/logger.hpp>
#include <bit>
#include <ranges>
#include <fmt/format.h>

namespace
{
	#ifdef GSL_ALLOCATIONS_TRACK
	using id_type = gal::gsl::utils::memory_model_tracker::id_type;

	id_type g_tracker = 0;
	id_type g_breakpoint = static_cast<id_type>(-1);
	#endif
}

namespace gal::gsl::utils
{
	#ifdef GSL_ALLOCATIONS_TRACK
	namespace memory_model_tracker
	{
		[[nodiscard]] auto generate_id() noexcept -> id_type { return g_tracker++; }

		auto set_breakpoint(const id_type id) noexcept -> void { g_breakpoint = id; }

		auto freeze_if_required() noexcept -> void { if (g_tracker == g_breakpoint) { debug::freeze_here(); } }
	}// namespace memory_model_tracker
	#endif

	namespace memory_model_detail
	{
		auto Hole::sweep() -> size_type
		{
			after_prey_end();

			size_type sweep_size = 0;

			// Divide the descriptor into several parts according to the `policy_type::aligned_bits`
			const size_type last_index = capacity_ / policy_type::aligned_bits;
			for (size_type i = 0; i < last_index; ++i)
			{
				// For each copy, there are `policy_type::aligned_bits` bits
				for (size_type offset = 0; offset < policy_type::aligned_bits; ++offset)
				{
					if (const auto mask = size_type{1} << offset;
						alive(i, mask))
					{
						// It is still alive
						sweep_size += size_per_prey_;
					}
					#ifdef GSL_ALLOCATIONS_SANITIZER
					else
					{
						// It is dead, clean the memory
						std::memset(hole_ + static_cast<std::ptrdiff_t>((i * policy_type::aligned_bits + offset) * size_per_prey_), 0xcd, size_per_prey_);
					}
					#endif
				}
			}
			return sweep_size;
		}

		auto Hole::sweep(const StackFunction<void(data_type, size_type)>& function) -> void
		{
			// Divide the descriptor into several parts according to the `policy_type::aligned_bits`
			const size_type last_index = capacity_ / policy_type::aligned_bits;
			for (size_type i = 0; i < last_index; ++i)
			{
				// For each copy, there are `policy_type::aligned_bits` bits
				for (size_type offset = 0; offset < policy_type::aligned_bits; ++offset)
				{
					if (const auto mask = size_type{1} << offset;
						alive(i, mask))
					{
						// It is still alive, call the function
						function(hole_ + static_cast<std::ptrdiff_t>((i * policy_type::aligned_bits + offset) * size_per_prey_), static_cast<size_type>(size_per_prey_));
					}
				}
			}
		}

		auto Hole::catch_new_one() -> data_type
		{
			if (caught_ == capacity_)
			{
				boost::logger::warn(
						"This hole is full(hole: {}, capacity: {}, size per prey: {}) and can't hold any more prey!",
						static_cast<void*>(hole_),
						capacity_,
						size_per_prey_);
				return nullptr;
			}

			// Divide the descriptor into several parts according to the `policy_type::aligned_bits`
			const size_type last_index = capacity_ / policy_type::aligned_bits;
			for (size_type i = 0; i < last_index; ++i)
			{
				// Get the bits of the descriptor currently in use
				const auto bits = hole_descriptor_[current_descriptor_index_];

				if (const auto flip_bits = ~bits)
				{
					// `flip_bits` are not equal to 0, which means that there are 0 (dead) bit(s) in `bits`, that is, these bits that can be used again
					// Find the first bit in `flip_bits` that is the 1 (dead) bit
					const auto first_one = std::countl_one(flip_bits);
					// cast to `index`, -1 ==> (count -> index)
					const auto bit_index = policy_type::aligned_bits - first_one - 1;

					// mark bit alive
					hole_descriptor_[current_descriptor_index_] |= (size_type{1} << bit_index);
					++caught_;

					const auto hole_index = (current_descriptor_index_ * policy_type::aligned_bits + bit_index);
					gsl_assert(hole_index < capacity_, fmt::format("Index({}) out of bound! This hole's capacity is {}(at {}, size per prey is {})", hole_index, capacity_, static_cast<void*>(hole_), size_per_prey_));
					return hole_ + static_cast<std::ptrdiff_t>(hole_index * size_per_prey_);
				}
			}

			gsl_trap("The hole did not catch enough prey, but the descriptor reported that the hole was full");
			return nullptr;
		}

		auto Hole::digest_this_one(const data_type prey) -> void
		{
			const auto [index, offset, mask] = policy_type::get_descriptor_state(hole_, capacity_, size_per_prey_, prey);
			gsl_assert(policy_type::descriptor_state{index, offset, mask} != policy_type::bad_descriptor, "Invalid descriptor state, maybe it's the prey that doesn't belong to the hole?");

			gsl_assert(alive(current_descriptor_index_, mask), "The hole can't digest a digested prey!");

			// flip that bit
			hole_descriptor_[current_descriptor_index_] ^= mask;
			current_descriptor_index_ = index;
			--caught_;
		}

		auto Hole::mark(const data_type prey) -> void
		{
			const auto [index, offset, mask] = policy_type::get_descriptor_state(hole_, capacity_, size_per_prey_, prey);
			gsl_assert(policy_type::descriptor_state{index, offset, mask} != policy_type::bad_descriptor, "Invalid descriptor state, maybe it's the prey that doesn't belong to the hole?");

			// Like `alive`, but this time we use `gc_hole_descriptor_` instead of `hole_descriptor_`
			if (!(gc_hole_descriptor_[current_descriptor_index_] & mask))
			{
				// dead
				gc_hole_descriptor_[index] |= mask;
				++gc_caught_;
			}
		}

		auto Trap::touch_hole(const size_type which_hole) const noexcept -> const Hole*
		{
			gsl_assert(which_hole < holes_.size(), "Index out of bound, did you forget to call `which_hole_for_prey` ?");

			return holes_[which_hole];
		}

		auto Trap::dig_hole(const size_type which_hole, const size_type capacity, const size_type size_per_prey, Hole* next) -> Hole*
		{
			gsl_assert(which_hole < holes_.size(), "Index out of bound, did you forget to call `which_hole_for_prey` ?");
			gsl_assert(size_per_prey <= heap_small_allocation_threshold, fmt::format("An allocation of size `{}` cannot be considered a small object allocation.", size_per_prey));

			return holes_[which_hole] = heap::new_object<Hole>(capacity, size_per_prey, next);
		}

		auto Trap::catch_new_one(const size_type size_per_prey) -> Trap::data_type
		{
			const auto real_size = policy_type::get_fit_aligned_size(size_per_prey);
			gsl_assert(real_size <= heap_small_allocation_threshold, fmt::format("An allocation of size `{}` cannot be considered a small object allocation.", size_per_prey));

			const auto which_hole = which_hole_for_prey(real_size);
			for (auto*& hole = holes_[which_hole]; hole; hole = hole->next()) { if (auto* data = hole->catch_new_one()) { return data; } }

			boost::logger::warn("This trap is full and can't hold any more prey!");
			return nullptr;
		}

		auto Trap::digest_this_one(const data_type prey, size_type size_per_prey) -> void
		{
			const auto real_size = policy_type::get_fit_aligned_size(size_per_prey);
			gsl_assert(real_size <= heap_small_allocation_threshold, fmt::format("An allocation of size `{}` cannot be considered a small object allocation.", size_per_prey));

			const auto which_hole = which_hole_for_prey(real_size);
			for (auto*& hole = holes_[which_hole]; hole; hole = hole->next()) { if (hole->inside(prey)) { hole->digest_this_one(prey); } }

			gsl_trap(fmt::format("Can't digest this prey({}), maybe the size({}) mismatch?", static_cast<void*>(prey), size_per_prey));
		}

		auto Trap::mark(const data_type prey, size_type size_per_prey) -> bool
		{
			const auto real_size = policy_type::get_fit_aligned_size(size_per_prey);
			gsl_assert(real_size <= heap_small_allocation_threshold, fmt::format("An allocation of size `{}` cannot be considered a small object allocation.", size_per_prey));

			const auto which_hole = which_hole_for_prey(real_size);
			for (auto*& hole = holes_[which_hole]; hole; hole = hole->next())
			{
				if (hole->inside(prey))
				{
					hole->mark(prey);
					return true;
				}
			}

			return false;
		}
	}

	FixedChunkModel::fixed_chunk::fixed_chunk(const size_type capacity, fixed_chunk* next)
		: size{0},
		capacity{policy_type::get_fit_aligned_size(capacity)},
		data{static_cast<data_type>(heap::allocate(capacity))},
		next{next} {}

	FixedChunkModel::fixed_chunk::~fixed_chunk() noexcept
	{
		heap::deallocate(data);

		while (next)
		{
			auto* to_delete = next;
			next = next->next;
			to_delete->next = nullptr;
			heap::delete_object(to_delete);
		}
	}

	auto FixedChunkModel::fixed_chunk::allocate(const size_type this_size) -> data_type
	{
		if (this_size + size > capacity) { return nullptr; }

		auto* dest = data + size;
		size += this_size;
		return dest;
	}

	auto FixedChunkModel::fixed_chunk::deallocate(const data_type this_data, const size_type this_size) -> void { if (this_data + this_size == data + size) { size -= this_size; } }

	auto FixedChunkModel::do_allocate(const size_type size) -> data_type
	{
		if (size == 0) { return nullptr; }

		const auto real_size = policy_type::get_fit_aligned_size(size);
		if (!chunk_) { chunk_ = heap::new_object<fixed_chunk>(std::ranges::max(initial_size_, real_size), nullptr); }

		for (;;)
		{
			if (auto* data = chunk_->allocate(size)) { return data; }
			chunk_ = heap::new_object<fixed_chunk>(std::ranges::max(do_hole_size_if_grow(chunk_->capacity), real_size), chunk_);
		}
	}

	auto FixedChunkModel::do_deallocate(const data_type data, const size_type size) -> bool
	{
		const auto real_size = policy_type::get_fit_aligned_size(size);

		for (auto*& c = chunk_; c; c = c->next)
		{
			if (c->inside(data))
			{
				c->deallocate(data, real_size);
				break;
			}
		}
		return true;
	}

	auto FixedChunkModel::do_reset() -> void
	{
		if (chunk_)
		{
			if (chunk_->next)
			{
				// Only one chunk has insufficient capacity

				// todo: aligned size?
				using policy = HeapAllocationAlignedPolicy<1024>;

				const auto max_allocated = policy_type::get_fit_aligned_size(used_memory());
				initial_size_ = std::ranges::max(initial_size_, max_allocated);
				heap::delete_object(chunk_);
				chunk_ = nullptr;
			}
			else
			{
				// One chunk is enough
				chunk_->size = 0;
			}
		}
	}

	FreeGrowModel::~FreeGrowModel() noexcept
	{
		// destroy trap
		trap_.destroy();

		// clear big stuff
		for (auto* data: big_stuffs_ | std::views::keys) { heap::deallocate(data); }
		big_stuffs_.clear();

		#ifdef GSL_ALLOCATIONS_SANITIZER
		// clear deleted big stuff
		for (auto* data: deleted_big_stuffs_ | std::views::keys) { heap::deallocate(data); }
		deleted_big_stuffs_.clear();
		#endif
	}

	auto FreeGrowModel::do_hole_size_if_grow(const size_type which_hole) const noexcept -> size_type
	{
		if (const auto* hole = trap_.touch_hole(which_hole))
		{
			auto capacity = hole->capacity();
			if (grow_function_) { capacity = grow_function_(capacity); }
			else { capacity *= default_grow_multiplier; }
			return capacity;
		}

		// fit in initial size
		return initial_size_ / trap_type::prey_size_in_hole(which_hole);
	}

	auto FreeGrowModel::do_allocated_memory() const noexcept -> size_type
	{
		auto allocated = trap_.state().total_space_for_holes;
		for (const auto size: big_stuffs_ | std::views::values) { allocated += size; }
		return allocated;
	}

	auto FreeGrowModel::do_allocate(const size_type size) -> data_type
	{
		if (size == 0) { return nullptr; }

		const auto real_size = trap_type::policy_type::get_fit_aligned_size(size);

		total_allocated_ += real_size;
		allocate_peak_ = std::ranges::max(allocate_peak_, total_allocated_);

		#ifndef GSL_ALLOCATIONS_TRACK
		// big stuff
		if (real_size > kHeapSmallAllocationThreshold)
		#endif
		{
			auto* data = static_cast<data_type>(heap::allocate(real_size));
			big_stuffs_[data] = real_size;
			#ifdef GSL_ALLOCATIONS_TRACK
			memory_model_tracker::freeze_if_required();
			big_stuff_infos_[data].id = memory_model_tracker::generate_id();
			#endif
			return data;
		}

		#ifndef GSL_ALLOCATIONS_TRACK
		// small stuff
		if (auto* data = trap_.catch_new_one(real_size)) { return data; }

		// allocate failed, make new hole
		const auto index		  = memory_model_detail::Trap::which_hole_for_prey(real_size);
		const auto elements_count = hole_size_if_grow(index);
		return trap_.dig_hole(index, elements_count, real_size, const_cast<memory_model_detail::Hole*>(trap_.touch_hole(index)))->catch_new_one();
		#endif
	}

	auto FreeGrowModel::do_deallocate(data_type data, const size_type size) -> bool
	{
		if (size == 0) { return true; }

		const auto real_size = trap_type::policy_type::get_fit_aligned_size(size);

		#ifdef GSL_ALLOCATIONS_SANITIZER
		std::memset(data, 0xcd, real_size);
		#endif

		#ifndef GSL_ALLOCATIONS_TRACK
		if (real_size <= kHeapSmallAllocationThreshold)
		{
			trap_.digest_this_one(data, size);
			total_allocated_ -= size;
			return true;
		}
		#endif

		#ifdef GSL_ALLOCATIONS_SANITIZER
		if (deleted_big_stuffs_.contains(data)) { debug::freeze_here(); }
		#endif

		if (auto big_stuff_it = big_stuffs_.find(data);
			big_stuff_it != big_stuffs_.end())
		{
			gsl_assert(big_stuff_it->second == size, fmt::format("Free size mismatch, {} allocated vs {} freed", big_stuff_it->second, size));

			#ifdef GSL_ALLOCATIONS_SANITIZER
			deleted_big_stuffs_.emplace(big_stuff_it->first, big_stuff_it->second);
			#else
			heap::deallocate(big_stuff_it->first);
			#endif

			big_stuffs_.erase(big_stuff_it);
			total_allocated_ -= size;

			#ifdef GSL_ALLOCATIONS_TRACK
			big_stuff_infos_.erase(data);
			#endif

			return true;
		}

		gsl_trap("unable to deallocate memory which not allocated by us");
		return false;
	}

	auto FreeGrowModel::do_reset() -> void
	{
		for (auto& [data, size]: big_stuffs_)
		{
			#ifdef GSL_ALLOCATIONS_SANITIZER
			deleted_big_stuffs_.emplace(data, size);
			#else
			heap::deallocate(data);
			#endif
		}
		big_stuffs_.clear();

		#ifdef GSL_ALLOCATIONS_TRACK
		big_stuff_infos_.clear();
		#endif

		trap_.destroy();
	}

	auto FreeGrowModel::do_dump() -> void
	{
		// todo: our logger
	}

	auto FreeGrowModel::mark(const data_type data, size_type size) -> void
	{
		// big stuff
		if (const auto it = big_stuffs_.find(data);
			it != big_stuffs_.end())
		{
			it->second |= heap_gc_mask;
			return;
		}

		if (size > heap_small_allocation_threshold) { return; }

		// small allocation
		if (!trap_.mark(data, size)) { boost::logger::warn("Cannot mark data(address: {}, size: {}).", static_cast<void*>(data), size); }
	}

	auto FreeGrowModel::sweep() -> void
	{
		#ifndef GSL_ALLOCATIONS_TRACK
		total_allocated_ = trap_.sweep();
		#else
		total_allocated_ = 0;
		#endif

		for (auto big_stuff_it = big_stuffs_.begin(); big_stuff_it != big_stuffs_.end();)
		{
			if (big_stuff_it->second & heap_gc_mask)
			{
				big_stuff_it->second &= ~heap_gc_mask;
				total_allocated_ += big_stuff_it->second;
				++big_stuff_it;
			}
			else
			{
				#ifdef GSL_ALLOCATIONS_SANITIZER
				std::memset(big_stuff_it->first, 0xcd, big_stuff_it->second);
				#endif

				heap::deallocate(big_stuff_it->first);
				big_stuff_it = big_stuffs_.erase(big_stuff_it);
			}
		}
	}
}// namespace gal::gsl::utils
