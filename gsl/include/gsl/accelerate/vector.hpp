#pragma once

#include <vector>
#include <gsl/boost/allocator.hpp>

namespace gal::gsl::accelerate
{
	template<typename T>
	using vector = ::std::vector<T, boost::gsl_allocator<T>>;
}
