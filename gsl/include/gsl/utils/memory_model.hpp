#pragma once

#include <gsl/config.hpp>

namespace gal::gsl::utils
{
	#ifdef GSL_ALLOCATIONS_TRACK
	namespace tracker
	{
		using id_type = std::uint64_t;

		[[nodiscard]] auto generator_id() noexcept -> id_type;

		auto set_breakpoint(id_type id) noexcept -> void;

		[[nodiscard]] auto require_freeze() noexcept -> bool;
	}
	#endif

	namespace memory_model_detail
	{
		class Hole
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
				: capacity_{policy_type::get_fit_capacity(capacity)},
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
			[[nodiscard]] constexpr auto alive(const size_type index, const size_type mask) const noexcept { return hole_descriptor_[index] & mask; }

		public:
			/**
			 * \brief Are the prey in the hole still alive?
			 * \param prey the prey
			 * \return it is still alive
			 * \note It is meaningful only when the prey is in the hole, otherwise the program may crash!
			 */
			[[nodiscard]] constexpr auto alive(const data_type prey) const noexcept
			{
				const auto [index, offset, mask] = policy_type::get_descriptor_state(hole_, capacity_, size_per_prey_, prey);
				return alive(index, mask);
			}

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
	}
}
