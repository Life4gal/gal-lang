#pragma once

#include <string_view>

namespace gal::gsl::accelerate
{
	template<typename Char, typename CharTraits = ::std::char_traits<Char>>
	using basic_string_view = ::std::basic_string_view<Char, CharTraits>;

	using string_view = basic_string_view<char>;
}
