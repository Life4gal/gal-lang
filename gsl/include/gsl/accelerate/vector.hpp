#pragma once

#include <vector>

namespace gal::gsl::accelerate
{
	template<typename T>
	using vector = ::std::vector<T, boost::gsl_allocator<T>>;
}
