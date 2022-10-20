#include <gsl/core/stack_allocator.hpp>

#include <gsl/utils/assert.hpp>

namespace gal::gsl::core
{
	StackAllocator::~StackAllocator() { destroy(); }

	auto StackAllocator::push(const size_type size) -> std::pair<bool, stack_state>
	{
		gsl_assert(stack_, "Stack is invalid, create a stack first!");

		if (usable() < size) { return {false, {}}; }

		auto ret = std::make_pair(true, state_);

		state_.allocation_top -= size;
		state_.evaluation_top = state_.allocation_top;

		return ret;
	}

	auto StackAllocator::push_invoke(const size_type size, const size_type stack_offset) -> std::pair<bool, stack_state>
	{
		// push
		gsl_assert(stack_, "Stack is invalid, create a stack first!");

		if (usable() < size) { return {false, {}}; }

		auto ret = std::make_pair(true, state_);

		state_.allocation_top -= size;
		// invoke
		state_.evaluation_top = end() + stack_offset;

		return ret;
	}
}
