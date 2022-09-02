#pragma once

// Do not include any header files, the allocator is always valid where this file is used.

namespace gal::gsl::boost
{
	template<typename T>
	using gsl_allocator = std::allocator<T>;
}
