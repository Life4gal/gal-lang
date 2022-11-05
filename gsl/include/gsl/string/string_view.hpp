#pragma once

#include <string_view>

namespace gal::gsl::string
{
	using string_view = std::basic_string_view<char, std::char_traits<char>>;

	// ReSharper disable once CppInconsistentNaming
	using u8string_view = std::basic_string_view<char8_t, std::char_traits<char8_t>>;
	// ReSharper disable once CppInconsistentNaming
	using u16string_view = std::basic_string_view<char16_t, std::char_traits<char16_t>>;
	// ReSharper disable once CppInconsistentNaming
	using u32string_view = std::basic_string_view<char32_t, std::char_traits<char32_t>>;
	// ReSharper disable once IdentifierTypo
	using wstring_view = std::basic_string_view<wchar_t, std::char_traits<wchar_t>>;
}
