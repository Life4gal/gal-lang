#pragma once

#include <vector>
#include <gsl/memory/allocator.hpp>

namespace gal::gsl::container
{
	template<typename T>
	using vector = std::vector<T, memory::StlAllocator<T>>;
}
