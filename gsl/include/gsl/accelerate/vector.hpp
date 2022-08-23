#pragma once

#include <vector>

namespace gal::gsl::accelerate
{
	template<typename T, typename Allocator = ::std::allocator<T>>
	using vector = ::std::vector<T, Allocator>;
}
