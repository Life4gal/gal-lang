#pragma once

#include <array>
#include <functional>
#include <gsl/accelerate/unordered_map.hpp>
#include <gsl/config.hpp>

namespace gal::gsl::core
{
	class LineInfo;
}

namespace gal::gsl::utils
{
	#ifdef GSL_ALLOCATIONS_TRACK
	namespace tracker
	{
		using id_type = std::uint64_t;

		[[nodiscard]] auto generate_id() noexcept -> id_type;

		auto set_breakpoint(id_type id) noexcept -> void;

		[[nodiscard]] auto require_freeze() noexcept -> bool;
	}
	#endif

	namespace memory_model_detail
	{
		class Hole final
		{
		public:
			using policy_type = heap_small_allocation_policy;

			using data_type = policy_type::data_type;
			using size_type = policy_type::size_type;

		private:
			size_type capacity_;
			size_type size_per_prey_;

			data_type hole_;

			size_type* hole_descriptor_;
			size_type* gc_hole_descriptor_;

			size_type caught_;
			size_type gc_caught_;

			size_type current_descriptor_index_;

			Hole* next_;

		public:
			Hole(const size_type capacity, const size_type size_per_prey, Hole* next)
				: capacity_{policy_type::get_fit_aligned_size(capacity)},
				size_per_prey_{size_per_prey},
				hole_{static_cast<data_type>(heap::allocate(capacity_ * size_per_prey_))},
				hole_descriptor_{static_cast<size_type*>(heap::allocate(policy_type::get_descriptor_count(capacity_)))},
				gc_hole_descriptor_{nullptr},
				caught_{0},
				gc_caught_{0},
				current_descriptor_index_{0},
				next_{next} { std::memset(hole_descriptor_, 0, policy_type::get_descriptor_count(capacity_)); }

			Hole(const Hole& other) = delete;
			Hole(Hole&& other) noexcept = delete;
			auto operator=(const Hole& other) -> Hole& = delete;
			auto operator=(Hole&& other) noexcept -> Hole& = delete;

			~Hole()
			{
				heap::deallocate(hole_);
				heap::deallocate(hole_descriptor_);
				if (next_) { heap::delete_object(next_); }
			}

			/**
			 * \brief Reset the entire hole, clearing all data (but not deallocate memory)
			 */
			auto reset() -> void
			{
				std::memset(hole_descriptor_, 0, policy_type::get_descriptor_count(capacity_));
				current_descriptor_index_ = 0;
				caught_ = 0;
				if (next_) { next_->reset(); }
			}

			/**
			 * \brief Preparation for prey
			 */
			auto before_prey_begin() -> void
			{
				gc_hole_descriptor_ = static_cast<size_type*>(heap::allocate(policy_type::get_descriptor_count(capacity_)));
				std::memset(gc_hole_descriptor_, 0, policy_type::get_descriptor_count(capacity_));
				current_descriptor_index_ = 0;
				gc_caught_ = 0;
				if (next_) { next_->before_prey_begin(); }
			}

			/**
			 * \brief Settlement of prey
			 */
			auto after_prey_end() -> void
			{
				heap::deallocate(hole_descriptor_);
				hole_descriptor_ = std::exchange(gc_hole_descriptor_, nullptr);
				caught_ = std::exchange(gc_caught_, 0);
			}

			/**
			 * \brief Whether the prey falls into the hole
			 * \param prey the prey
			 * \return if it falls into a hole, returns true; otherwise, returns false
			 */
			[[nodiscard]] constexpr auto inside(const data_type prey) const noexcept -> bool { return prey >= hole_ && prey < hole_ + static_cast<std::ptrdiff_t>(caught_ * size_per_prey_); }

		private:
			[[nodiscard]] constexpr auto alive(const size_type index, const size_type mask) const noexcept -> bool { return hole_descriptor_[index] & mask; }

		public:
			/**
			 * \brief Are the prey in the hole still alive?
			 * \param prey the prey
			 * \return it is still alive
			 * \note It is meaningful only when the prey is in the hole, otherwise the program may crash!
			 */
			[[nodiscard]] constexpr auto alive(const data_type prey) const noexcept -> bool
			{
				const auto [index, offset, mask] = policy_type::get_descriptor_state(hole_, capacity_, size_per_prey_, prey);
				return alive(index, mask);
			}

			/**
			 * \brief Sweep the hole and count the prey alive after prey end
			 * \return the prey count (* size_per_element)
			 */
			[[nodiscard]] auto sweep() -> size_type;

			/**
			 * \brief Obtain the number of prey currently captured (only the alive ones are included)
			 * \return the number of prey currently captured
			 */
			[[nodiscard]] constexpr auto size() const noexcept -> size_type { return caught_; }

			/**
			 * \brief Obtain the upper limit of prey that can be caught in the current hole
			 * \return the upper limit of prey that can be caught in the current hole
			 */
			[[nodiscard]] constexpr auto capacity() const noexcept -> size_type { return capacity_; }

			/**
			 * \brief Obtain the space spent by the current hole due to prey
			 * \return the space spent by the current hole due to prey
			 */
			[[nodiscard]] constexpr auto space_for_prey() const noexcept -> size_type { return size() * size_per_prey_; }

			/**
			 * \brief Obtain the space spent for the composition of the current entire hole
			 * \return the space spent for the composition of the current entire hole
			 */
			[[nodiscard]] constexpr auto space_for_hole() const noexcept -> size_type
			{
				// hole + descriptor
				return capacity() * size_per_prey_ + policy_type::get_descriptor_count(capacity_);
			}

			/**
			 * \brief Get the next hole connected by the current hole
			 * \return the next hole
			 */
			[[nodiscard]] constexpr auto next() const -> Hole* { return next_; }

			/**
			 * \brief Capture a new prey
			 * \return a new prey
			 */
			[[nodiscard]] auto catch_new_one() -> data_type;

			/**
			 * \brief Digest a prey
			 * \param prey a prey
			 * \note It is meaningful only when the prey is in the hole, otherwise the program may crash!
			 */
			auto digest_this_one(data_type prey) -> void;

			/**
			 * \brief Mark a prey for counting dead prey
			 * \param prey a prey
			 * \note It is meaningful only when the prey is in the hole, otherwise the program may crash!
			 */
			auto mark(data_type prey) -> void;
		};

		class Trap final
		{
		public:
			using data_type = Hole::data_type;
			using size_type = Hole::size_type;

			constexpr static size_type size_shift = 4;
			constexpr static size_type hole_size = kHeapSmallAllocationThreshold >> size_shift;
			constexpr static size_type size_per_hole = kHeapSmallAllocationThreshold / hole_size;

			// todo: adaptive?
			using policy_type = heap_small_allocation_policy::rebind_policy<std::uint16_t>;
			static_assert(policy_type::size_type_bit_size == size_per_hole);

			[[nodiscard]] constexpr static auto prey_size_in_hole(const size_type which_hole) noexcept -> size_type { return (which_hole + 1) << size_shift; }

			[[nodiscard]] constexpr static auto which_hole_for_prey(const size_type size_of_prey) noexcept -> size_type { return (size_of_prey >> size_shift) - 1; }

			struct trap_state
			{
				size_type total_holes;
				size_type deepest_depth;
				size_type total_space_for_preys;
				size_type total_space_for_holes;
			};

		private:
			std::array<Hole*, hole_size> holes_;

		public:
			Trap() = default;

			Trap(const Trap& other) = delete;
			Trap(Trap&& other) noexcept = delete;
			auto operator=(const Trap& other) -> Trap& = delete;
			auto operator=(Trap&& other) noexcept -> Trap& = delete;

			~Trap() noexcept { destroy(); }

			/**
			 * \brief destroy the whole trap (deallocate memory)
			 */
			auto destroy() -> void { for (auto*& hole: holes_) { heap::delete_object(hole); } }

			/**
			 * \brief reset all holes (not deallocate memory)
			 */
			auto reset() -> void { for (auto*& hole: holes_) { hole->reset(); } }

			/**
			 * \brief Whether the prey falls into the trap
			 * \param prey the prey
			 * \param size_per_prey the size of the prey
			 * \return if it falls into the trap, returns true; otherwise, returns false
			 */
			[[nodiscard]] constexpr auto inside(const data_type prey, const size_type size_per_prey) const noexcept -> bool
			{
				const auto which_hole = which_hole_for_prey(size_per_prey);
				for (const auto* hole = holes_[which_hole]; hole; hole = hole->next()) { if (hole->inside(prey)) { return true; } }
				return false;
			}

			/**
			 * \brief Are the prey in the trap still alive?
			 * \param prey the prey
			 * \param size_per_prey the size of the prey
			 * \return if it falls into the trap and it still alive, returns true; otherwise, returns false
			 */
			[[nodiscard]] constexpr auto alive(const data_type prey, const size_type size_per_prey) const noexcept -> bool
			{
				const auto which_hole = which_hole_for_prey(size_per_prey);
				for (const auto* hole = holes_[which_hole]; hole; hole = hole->next()) { if (hole->inside(prey)) { return hole->alive(prey); } }
				return false;
			}

			/**
			 * \brief Sweep the holes and count the prey alive after prey end
			 * \return the prey count (* size_per_element)
			 */
			[[nodiscard]] auto sweep() -> size_type
			{
				size_type sweep_size = 0;
				for (auto*& hole: holes_) { for (; hole; hole = hole->next()) { sweep_size += hole->sweep(); } }
				return sweep_size;
			}

			/**
			 * \brief Get state about current trap
			 * \return the state about current trap
			 */
			[[nodiscard]] constexpr auto state() const noexcept -> trap_state
			{
				trap_state ret{};
				for (const auto* hole: holes_)
				{
					size_type current_depth = 0;
					for (; hole; hole = hole->next())
					{
						++current_depth;

						ret.total_holes += 1;
						ret.total_space_for_preys += hole->space_for_prey();
						ret.total_space_for_holes += hole->space_for_hole();
					}

					if (current_depth > ret.deepest_depth) { ret.deepest_depth = current_depth; }
				}
				return ret;
			}

			[[nodiscard]] auto touch_hole(size_type which_hole) const noexcept -> const Hole*;

			[[nodiscard]] auto dig_hole(size_type which_hole, size_type capacity, size_type size_per_prey, Hole* next) -> Hole*;

			/**
			 * \brief Capture a new prey
			 * \param size_per_prey the prey size
			 * \return a new prey
			 */
			[[nodiscard]] auto catch_new_one(size_type size_per_prey) -> data_type;

			/**
			 * \brief Digest a prey
			 * \param prey a prey
			 * \param size_per_prey the size of the prey
			 * \note It is meaningful only when the prey is in the trap, otherwise the program may crash!
			 */
			auto digest_this_one(data_type prey, size_type size_per_prey) -> void;

			/**
			 * \brief Mark a prey for counting dead prey
			 * \param prey a prey
			 * \param size_per_prey the size of the prey
			 * \return returns true if the prey is in the trap and successfully marked the prey, otherwise returns false
			 */
			auto mark(data_type prey, size_type size_per_prey) -> bool;

			/**
			 * \brief Preparation for prey
			 */
			auto before_prey_begin() -> void { for (auto*& hole: holes_) { hole->before_prey_begin(); } }
		};
	}

	class MemoryModel final
	{
	public:
		using policy_type = heap_small_allocation_policy;

		using data_type = policy_type::data_type;
		using size_type = policy_type::size_type;

		using grow_function_type = std::function<size_type(size_type)>;

		using trap_type = memory_model_detail::Trap;

		constexpr static size_type default_grow_multiplier = 2;
		constexpr static size_type default_initial_size = 1 << 16;

	private:
		/**
		 * \brief Initial capacity of the hole (divided by the size of the prey)
		 */
		size_type initial_size_;

		/**
		 * \brief The total amount of memory currently used
		 */
		size_type total_allocated_;

		/**
		 * \brief Peak memory usage
		 */
		size_type allocate_peak_;

		/**
		 * \brief Function to replace the default growth method (multiply by default_grow_multiplier)
		 */
		grow_function_type grow_function_;

		/**
		 * \brief The trap for small allocations
		 */
		trap_type trap_;

		/**
		 * \brief The big stuffs
		 */
		accelerate::unordered_map<void*, size_type> big_stuffs_;

		#ifdef GSL_ALLOCATIONS_TRACK
	public:
		struct big_stuff_info
		{
			using comment_type = const char*;

			tracker::id_type id;
			core::LineInfo* location;
			comment_type comment;
		};

	private:
		accelerate::unordered_map<void*, big_stuff_info> big_stuff_infos_;
		#endif

		#ifdef GSL_ALLOCATIONS_SANITIZER
		accelerate::unordered_map<void*, size_type> deleted_big_stuffs_;
		#endif

	public:
		explicit MemoryModel(const size_type initial_size = default_initial_size)
			: initial_size_{initial_size},
			total_allocated_{0},
			allocate_peak_{0},
			trap_{} {}

		MemoryModel(const MemoryModel& other) = delete;
		MemoryModel(MemoryModel&& other) noexcept = delete;
		auto operator=(const MemoryModel& other) -> MemoryModel& = delete;
		auto operator=(MemoryModel&& other) noexcept -> MemoryModel& = delete;

		~MemoryModel() noexcept;

		/**
		 * \brief Reset the initial capacity of the hole (only for holes created later)
		 * \param initial_size the new initial_size
		 */
		constexpr auto set_initial_size(const size_type initial_size) noexcept -> void { initial_size_ = initial_size; }

		/**
		 * \brief Get the initial capacity of the hole
		 * \return the initial capacity of the hole
		 */
		[[nodiscard]] constexpr auto get_initial_size() const noexcept -> size_type { return initial_size_; }

		/**
		 * \brief Set the growth mode of hole capacity
		 * \param grow_function the growth mode of hole capacity
		 */
		auto set_grow_function(grow_function_type&& grow_function) noexcept -> void { grow_function_ = std::forward<decltype(grow_function)>(grow_function); }

		/**
		 * \brief If we want to grow the capacity of a hole, how much the capacity should be
		 * \param which_hole which hole we want to grow
		 * \return the new capacity
		 */
		[[nodiscard]] auto hole_size_if_grow(size_type which_hole) const noexcept -> size_type;

		/**
		 * \brief Determine if data is allocated by us
		 * \param data the data
		 * \param size_per_element the element size of data
		 * \return return true if it is, otherwise, return false
		 */
		[[nodiscard]] auto inside(const data_type data, const size_type size_per_element) const noexcept -> bool { return trap_.inside(data, size_per_element) || big_stuffs_.contains(data); }

		/**
		 * \brief Determine if data is allocated by us and it still valid
		 * \param data the data
		 * \param size_per_element the element size of data
		 * \return return true if it is, otherwise, return false
		 */
		[[nodiscard]] auto alive(const data_type data, const size_type size_per_element) const noexcept -> bool { return trap_.alive(data, size_per_element) || big_stuffs_.contains(data); }

		/**
		 * \brief Returns the amount of memory used from `allocate`
		 * \return the amount of memory used
		 */
		[[nodiscard]] constexpr auto used_memory() const noexcept -> size_type { return total_allocated_; }

		/**
		 * \brief Returns the peak amount of memory used from `allocate`
		 * \return the peak amount of memory used
		 */
		[[nodiscard]] constexpr auto peak_memory() const noexcept -> size_type { return allocate_peak_; }

		/**
		 * \brief Returns the depth of the deepest hole
		 * \return the depth of the deepest hole
		 */
		[[nodiscard]] auto deepest_depth() const noexcept -> size_type { return trap_.state().deepest_depth; }

		/**
		 * \brief Returns the sum of memory usage for the entire hole and big stuffs
		 * \return the sum of memory usage
		 */
		[[nodiscard]] auto allocated_memory() const noexcept -> size_type;

		#ifdef GSL_ALLOCATIONS_TRACK
		/**
		 * \brief Add some extra information to a big stuff, only meaningful for big stuff
		 * \param data the big stuff
		 * \param info extra information
		 */
		auto mark(const data_type data, big_stuff_info&& info) -> void { big_stuff_infos_.emplace(data, std::forward<decltype(info)>(info)); }
		#endif

		/**
		 * \brief Allocate a block of memory of size `size`
		 * \param size the size of block
		 * \return the memory block
		 */
		[[nodiscard]] auto allocate(size_type size) -> data_type;

		/**
		 * \brief Deallocate a block of memory of size `size`
		 * \param data the memory block
		 * \param size the size of block
		 * \return whether the deallocation was successful, this should always return true
		 */
		auto deallocate(data_type data, size_type size) -> bool;

		/**
		 * \brief Mark the target data (for GC)
		 * \param data the data
		 * \param size the size of the data
		 */
		auto mark(data_type data, size_type size) -> void;

		/**
		 * \brief prepare for GC
		 */
		auto prepare_for_gc() -> void { trap_.before_prey_begin(); }

		/**
		 * \brief clear all data (deallocate memory)
		 */
		auto reset() -> void;

		/**
		 * \brief Record all memory allocations
		 */
		auto sweep() -> void;

		/**
		 * \brief Dump all information about the model
		 */
		auto dump() -> void;
	};
}
