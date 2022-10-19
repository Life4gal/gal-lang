#pragma once

#include <gsl/utils/memory_model.hpp>
#include <gsl/utils/hash.hpp>
#include <gsl/utils/stack_function.hpp>
#include <gsl/utils/string_pool.hpp>
#include <gsl/accelerate/unordered_set.hpp>
#include <gsl/accelerate/string_view.hpp>

namespace gal::gsl::utils
{
	#ifdef GSL_ALLOCATIONS_TRACK
	namespace allocator_tracker
	{
		using id_type = std::uint64_t;

		[[nodiscard]] auto generate_id() noexcept -> id_type;

		auto set_breakpoint(id_type id) noexcept -> void;

		auto freeze_if_required() noexcept -> void;
	}// namespace allocator_tracker
	#endif

	class StackAllocator final
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

	class AllocatorBase : public accelerate::enable_shared_from_this<AllocatorBase>
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

	class StringAllocatorBase : public AllocatorBase
	{
	public:
		// todo: our value type
		using value_type = accelerate::string_view;
		using hasher_type = decltype([](const value_type& value) { return hash64(value); });
		using comparator_type = std::equal_to<void>;

		using list_type = accelerate::unordered_set<value_type, hasher_type, comparator_type>;

		// todo: for `recognize(value_type string)`
		using policy_type = memory_model_detail::Trap::policy_type;

		using pointer = value_type::pointer;
		using size_type = value_type::size_type;

	private:
		list_type intern_list_;
		bool intern_required_{false};

	public:
		/**
		 * \brief Construct a string from the given string
		 * \param string the given string
		 * \return the new string
		 * \note If the intern is not required, this is just a simple copy of a string. If it is required, it will search in intern_list (return the string directly if it is found), and add intern_list after construct (if it is not found)
		 */
		[[nodiscard]] auto construct(value_type string) -> value_type;

		/**
		 * \brief Destruct string constructed before
		 * \param string the string constructed before
		 * \note If the previously constructed string is not destructed, it may cause a memory leak
		 */
		auto destruct(value_type string) -> void;

		/**
		 * \brief Get whether intern is required currently
		 * \return whether intern is required currently
		 */
		[[nodiscard]] constexpr auto is_intern_required() const noexcept -> bool { return intern_required_; }

		/**
		 * \brief Set whether intern is required currently
		 * \param required whether intern is required currently
		 */
		constexpr auto set_intern_required(const bool required) noexcept -> void
		{
			intern_required_ = required;
			if (!intern_required_) { reset(); }
		}

		/**
		 * \brief Find the target string from intern_list, if found, return that string, otherwise return an empty string
		 * \param string the target string
		 * \return if found, return that string, otherwise return an empty string
		 */
		[[nodiscard]] auto intern(value_type string) -> value_type;

		/**
		 * \brief If the target string is allocated by us, record it in intern_list (if required), otherwise do nothing
		 * \param string the target string
		 */
		auto recognize(value_type string) -> void;

		auto reset() noexcept -> void override { intern_list_.clear(); }

		/**
		 * \brief Traverse all strings
		 * \param function how to handle each string
		 */
		virtual auto for_each(const StackFunction<void(value_type)>& function) -> void = 0;
	};

	class ConstStringAllocator final : public accelerate::enable_shared_from_this<ConstStringAllocator>
	{
	public:
		using pool_type = StringPool;
		using value_type = pool_type::value_type;

		using list_type = StringAllocatorBase::list_type;
		static_assert(std::is_same_v<list_type::value_type, value_type>);

	private:
		pool_type pool_;
		list_type intern_list_;

	public:
		/**
		 * \brief Construct a string from the given string
		 * \param string the given string
		 * \return the new string
		 * \note It will search in intern_list (return the string directly if it is found), and add intern_list after construct (if it is not found)
		 */
		[[nodiscard]] auto construct(value_type string) -> value_type;

		/**
		 * \brief Clear all constructed strings (deallocate memory)
		 */
		auto clear() noexcept -> void
		{
			intern_list_.clear();
			pool_.clear();
		}

		/**
		 * \brief Find the target string from intern_list, if found, return that string, otherwise return an empty string
		 * \param string the target string
		 * \return if found, return that string, otherwise return an empty string
		 */
		[[nodiscard]] auto intern(value_type string) -> value_type;
	};
}
