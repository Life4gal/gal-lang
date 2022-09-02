#pragma once

#include <string>
#include <gsl/boost/allocator.hpp>

namespace gal::gsl::accelerate
{
	template<typename Char, typename CharTraits = ::std::char_traits<Char>>
	using basic_string = ::std::basic_string<Char, CharTraits, boost::gsl_allocator<char>>;

	using string = basic_string<char>;
}
