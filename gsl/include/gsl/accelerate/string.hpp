#pragma once

#include <string>

namespace gal::gsl::accelerate
{
	template<typename Char, typename CharTraits = ::std::char_traits<Char>, typename Allocator = ::std::allocator<Char>>
	using basic_string = ::std::basic_string<Char, CharTraits, Allocator>;

	using string = basic_string<char>;
}
