#include <gsl/simulate/simulate.hpp>

#include <gsl/debug/assert.hpp>

#include <stdexcept>

namespace gal::gsl::simulate
{
	SimulateNode::~SimulateNode() noexcept = default;

	SimulateContext::SimulateContext()
		: function_arguments_{nullptr},
		  chunk_{any_allocator_.allocate(chunk_size), chunk_size},
		  current_chunk_used_{0},
		  stack_{any_allocator_.allocate(stack_size), stack_size},
		  current_stack_used_{0} {}

	SimulateContext::~SimulateContext() noexcept
	{
		any_allocator_.deallocate(chunk_.data(), chunk_size);
		any_allocator_.deallocate(stack_.data(), stack_size);
	}

	auto SimulateContext::allocate(chunk_type::size_type size) -> chunk_type::pointer
	{
		// min size per allocate
		constexpr chunk_type::size_type min_size_per_allocate = 1 << 4;
		constexpr chunk_type::size_type min_size_per_allocate_mask = min_size_per_allocate - 1;

		size = (size + min_size_per_allocate_mask) & ~min_size_per_allocate_mask;

		if (current_chunk_used_ + size > chunk_size) { throw std::runtime_error{"out of chunk space"}; }

		auto* result = chunk_.data() + current_chunk_used_;
		current_chunk_used_ += size;
		return result;
	}

	auto SimulateContext::call_function(const function_index_type index, SimulateNode::value_type* args) -> SimulateNode::value_type
	{
		const auto& function = functions_[index];

		// push stack
		auto* prev_arguments = function_arguments_;
		const auto prev_stack = current_stack_used_;

		// set arguments
		function_arguments_ = args;
		if (current_stack_used_ + function.stack_size() > stack_size) { throw std::runtime_error{"stack overflow"}; }
		current_stack_used_ += function.stack_size();

		// call
		const auto ret = function.eval(*this);

		// pop stack
		function_arguments_ = prev_arguments;
		current_stack_used_ = prev_stack;

		return ret;
	}
}
