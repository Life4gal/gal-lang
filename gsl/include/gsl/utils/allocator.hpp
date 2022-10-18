#pragma once

#include <gsl/utils/memory_model.hpp>

namespace gal::gsl::utils
{
	class StackAllocator
	{
	public:
		using data_type = MemoryModel::data_type;
		using size_type = MemoryModel::size_type;

		struct stack_state
		{
			data_type allocation_top{nullptr};
			data_type evaluation_top{nullptr};
		};

	private:
		/**
		 * stack_size --> +-------+ <-- allocation_top&evaluation_top == 0 <-- empty() <-- top()
		 *                +       + <-- allocation_top&evaluation_top == 1 <- used() == 1
		 *                +       + ...
		 *                +       +
		 *                +       +
		 *                +       +
		 *                +       +
		 *                +       +
		 *                +       +
		 *                +       +
		 *                +       + 
		 *                +       + 
		 *                +-------+ <-- stack <-- bottom()
		 */

		size_type stack_size_;
		data_type stack_;

		stack_state state_;

	public:
		explicit StackAllocator(const size_type size)
			: stack_size_{size},
			stack_{stack_size_ ? static_cast<data_type>(heap::allocate(stack_size_)) : nullptr} { reset(); }

		StackAllocator(const StackAllocator& other) = delete;
		StackAllocator(StackAllocator&& other) noexcept = delete;
		auto operator=(const StackAllocator& other) -> StackAllocator& = delete;
		auto operator=(StackAllocator&& other) noexcept -> StackAllocator& = delete;

		virtual ~StackAllocator();

		/**
		 * \brief Destroy the entire stack (deallocate memory)
		 */
		auto destroy() -> void
		{
			if (stack_)
			{
				heap::deallocate(stack_);
				stack_ = nullptr;
			}
		}

		/**
		 * \brief Give up ownership (no longer share stack with other allocators, and no longer responsible for stack destruction)
		 */
		constexpr auto fade() noexcept -> void { stack_ = nullptr; }

		/**
		 * \brief Share a stack with other allocators (if the stack is currently held, nothing will be done to it)
		 * \param other allocator to share with
		 */
		constexpr auto shared_with(const StackAllocator& other) noexcept -> void
		{
			stack_size_ = other.stack_size_;
			stack_ = other.stack_;
			state_ = other.state_;
		}

	private:
		/**
		 * \brief It's actually end
		 * \return the end
		 */
		[[nodiscard]] constexpr auto begin() const noexcept -> data_type { return stack_ + stack_size_; }

		/**
		 * \brief It's actually begin
		 * \return the begin
		 */
		[[nodiscard]] constexpr auto end() const noexcept -> data_type { return stack_; }

		/**
		 * \brief Current stack top
		 * \return the stack top
		 */
		[[nodiscard]] constexpr auto current() const noexcept -> data_type { return state_.allocation_top; }

	public:
		/**
		 * \brief Determine whether stack space is not used
		 * \return if stack space is not used, return true; otherwise, return false
		 */
		[[nodiscard]] constexpr auto empty() const noexcept -> bool { return current() == begin(); }

		/**
		 * \brief Get the amount of stack space used
		 * \return the amount of stack space used
		 */
		[[nodiscard]] constexpr auto used() const noexcept -> size_type { return static_cast<size_type>(begin() - current()); }

		/**
		 * \brief Get the amount of stack space available
		 * \return the amount of stack space available
		 */
		[[nodiscard]] constexpr auto usable() const noexcept -> size_type { return static_cast<size_type>(current() - end()); }

		/**
		 * \brief Get the capacity of the entire stack space
		 * \return the capacity of the entire stack space
		 */
		[[nodiscard]] constexpr auto capacity() const noexcept -> size_type { return static_cast<size_type>(begin() - end()); }

		/**
		 * \brief Get the bottom of the stack
		 * \return the bottom of the stack
		 */
		[[nodiscard]] constexpr auto bottom() const noexcept -> data_type { return end(); }

		/**
		 * \brief Get the top of the stack
		 * \return the top of the stack
		 */
		[[nodiscard]] constexpr auto top() const noexcept -> data_type { return begin(); }

		[[nodiscard]] constexpr auto inside(const data_type data) const noexcept -> bool { return data >= bottom() && data < top(); }

		/**
		 * \brief Clear all used stack space
		 */
		constexpr auto reset() noexcept -> void { state_ = stack_state{begin(), begin()}; }

		/**
		 * \brief Get the state of the current stack
		 * \return the state of the current stack
		 */
		[[nodiscard]] constexpr auto state() const noexcept -> stack_state { return state_; }

		/**
		 * \brief Use a certain amount of stack space
		 * \return returns a key value pair.First indicates whether the push is successful, and second indicates the state of the stack before push
		 */
		[[nodiscard]] auto push(size_type size) -> std::pair<bool, stack_state>;

		/**
		 * \brief Return a certain amount of stack space
		 * \param prev_state the stack state previously obtained by calling push
		 */
		auto pop(const stack_state& prev_state) -> void { state_ = prev_state; }

		[[nodiscard]] constexpr auto invoke(const size_type stack_offset) -> stack_state
		{
			const auto ret = state_;

			state_.evaluation_top = end() + stack_offset;

			return ret;
		}

		auto push_invoke(size_type size, size_type stack_offset) -> std::pair<bool, stack_state>;
	};

	class AllocatorBase
	{
	public:
		using data_type = MemoryModel::data_type;
		using size_type = MemoryModel::size_type;

		AllocatorBase() = default;

		AllocatorBase(const AllocatorBase& other) = delete;
		AllocatorBase(AllocatorBase&& other) noexcept = delete;
		auto operator=(const AllocatorBase& other) -> AllocatorBase& = delete;
		auto operator=(AllocatorBase&& other) noexcept -> AllocatorBase& = delete;

		virtual ~AllocatorBase() = 0;

		constexpr virtual auto set_initial_size(size_type initial_size) noexcept -> void = 0;
		[[nodiscard]] constexpr virtual auto get_initial_size() const noexcept -> size_type = 0;
		virtual auto set_grow_function(MemoryModel::grow_function_type&& grow_function) -> void = 0;
		[[nodiscard]] virtual auto inside(data_type data, size_type size) const noexcept -> bool = 0;
		[[nodiscard]] virtual auto alive(data_type data, size_type size) const noexcept -> bool = 0;
		[[nodiscard]] virtual auto used_memory() const noexcept -> size_type = 0;
		[[nodiscard]] virtual auto peak_memory() const noexcept -> size_type = 0;
		[[nodiscard]] virtual auto allocated_memory() const noexcept -> size_type = 0;
		[[nodiscard]] virtual auto deepest_depth() const noexcept -> size_type = 0;
		#ifdef GSL_ALLOCATIONS_TRACK
		virtual auto mark(const data_type data, MemoryModel::big_stuff_info&& info) -> void {}
		#endif

		[[nodiscard]] virtual auto allocate(size_type size) -> data_type = 0;
		virtual auto deallocate(data_type data, size_type size) -> void = 0;
		virtual auto mark(data_type data, size_type size) -> void = 0;
		virtual auto prepare_for_gc() -> bool = 0;
		virtual auto reset() -> void = 0;
		virtual auto sweep() -> void = 0;

		virtual auto dump() -> void = 0;
	};

	class PersistentAllocator final : public AllocatorBase
	{
	public:
		using AllocatorBase::AllocatorBase;

	private:
		MemoryModel model_;

	public:
		constexpr auto set_initial_size(const size_type initial_size) noexcept -> void override { model_.set_initial_size(initial_size); }

		[[nodiscard]] constexpr auto get_initial_size() const noexcept -> size_type override { return model_.get_initial_size(); }

		auto set_grow_function(MemoryModel::grow_function_type&& grow_function) -> void override { model_.set_grow_function(std::forward<decltype(grow_function)>(grow_function)); }

		[[nodiscard]] auto inside(const data_type data, const size_type size) const noexcept -> bool override { return model_.inside(data, size); }

		[[nodiscard]] auto alive(const data_type data, const size_type size) const noexcept -> bool override { return model_.alive(data, size); }

		[[nodiscard]] auto used_memory() const noexcept -> size_type override { return model_.used_memory(); }

		[[nodiscard]] auto peak_memory() const noexcept -> size_type override { return model_.peak_memory(); }

		[[nodiscard]] auto allocated_memory() const noexcept -> size_type override { return model_.allocated_memory(); }

		[[nodiscard]] auto deepest_depth() const noexcept -> size_type override { return model_.deepest_depth(); }

		#ifdef GSL_ALLOCATIONS_TRACK
		auto mark(const data_type data, MemoryModel::big_stuff_info&& info) -> void override { model_.mark(data, std::forward<decltype(info)>(info)); }
		#endif

		auto allocate(const size_type size) -> data_type override { return model_.allocate(size); }

		auto deallocate(const data_type data, const size_type size) -> void override { model_.deallocate(data, size); }

		auto mark(const data_type data, const size_type size) -> void override { model_.mark(data, size); }

		auto prepare_for_gc() -> bool override
		{
			model_.prepare_for_gc();
			return true;
		}

		auto reset() -> void override { model_.reset(); }

		auto sweep() -> void override { model_.sweep(); }

		auto dump() -> void override { model_.dump(); }
	};
}
