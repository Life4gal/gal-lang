#include <gsl/utils/memory_model.hpp>

#include <gsl/utils/assert.hpp>
#include <gsl/boost/logger.hpp>
#include <bit>
#include <fmt/format.h>

namespace
{
	#ifdef GSL_ALLOCATIONS_TRACK
	using id_type = gal::gsl::utils::tracker::id_type;

	id_type g_tracker = 0;
	id_type g_breakpoint = static_cast<id_type>(-1);
	#endif
}

namespace gal::gsl::utils
{
	#ifdef GSL_ALLOCATIONS_TRACK
	namespace tracker
	{
		[[nodiscard]] auto generator_id() noexcept -> id_type { return g_tracker++; }

		auto set_breakpoint(const id_type id) noexcept -> void { g_breakpoint = id; }

		[[nodiscard]] auto require_freeze() noexcept -> bool { return g_tracker == g_breakpoint; }
	}// namespace tracker
	#endif

	namespace memory_model_detail
	{
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

			// todo
			const auto last = policy_type::get_descriptor_count(capacity_) / 4;
			for (size_type i = 0; i < last; ++i)
			{
				if (const auto bit = ~hole_descriptor_[current_descriptor_index_])
				{
					const auto offset = policy_type::size_type_mask - std::countl_zero(bit);
					hole_descriptor_[current_descriptor_index_] |= size_type{1} << offset;
					++caught_;
					const auto index = (current_descriptor_index_ * policy_type::size_type_bit_size + offset);

					gsl_assert(index < capacity_, fmt::format("Index({}) out of bound! This hole's capacity is {}(at {}, size per prey is {})", index, capacity_, static_cast<void*>(hole_), size_per_prey_));
					return hole_ + static_cast<std::ptrdiff_t>(index * size_per_prey_);
				}
				++current_descriptor_index_;
				if (current_descriptor_index_ == last) { current_descriptor_index_ = 0; }
			}

			gsl_trap("The hole did not catch enough prey, but the descriptor reported that the hole was full");
			return nullptr;
		}

		auto Hole::digest_this_one(const data_type prey) -> void
		{
			const auto [index, offset, mask] = policy_type::get_descriptor_state(hole_, capacity_, size_per_prey_, prey);
			gsl_assert(policy_type::descriptor_state{index, offset, mask} != policy_type::bad_descriptor, "Invalid descriptor state, maybe it's the prey that doesn't belong to the hole?");

			gsl_assert(hole_descriptor_[current_descriptor_index_] & mask, "The hole can't digest a digested prey!");

			hole_descriptor_[current_descriptor_index_] ^= mask;
			current_descriptor_index_ = index;
			--caught_;
		}

		auto Hole::mark(const data_type prey) -> void
		{
			const auto [index, offset, mask] = policy_type::get_descriptor_state(hole_, capacity_, size_per_prey_, prey);
			gsl_assert(policy_type::descriptor_state{index, offset, mask} != policy_type::bad_descriptor, "Invalid descriptor state, maybe it's the prey that doesn't belong to the hole?");

			if (!(hole_descriptor_[current_descriptor_index_] & mask))
			{
				// dead
				gc_hole_descriptor_[index] |= mask;
				++gc_caught_;
			}
		}
	}
}// namespace gal::gsl::utils
