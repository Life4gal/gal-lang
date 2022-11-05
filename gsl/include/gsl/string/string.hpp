#pragma once

#include <string>
#include <gsl/memory/allocator.hpp>

namespace gal::gsl::string
{
	using string = std::basic_string<char, std::char_traits<char>, memory::StlAllocator<char>>;

	// ReSharper disable once CppInconsistentNaming
	using u8string = std::basic_string<char8_t, std::char_traits<char8_t>, memory::StlAllocator<char8_t>>;
	// ReSharper disable once CppInconsistentNaming
	using u16string = std::basic_string<char16_t, std::char_traits<char16_t>, memory::StlAllocator<char16_t>>;
	// ReSharper disable once CppInconsistentNaming
	using u32string = std::basic_string<char32_t, std::char_traits<char32_t>, memory::StlAllocator<char32_t>>;
	// ReSharper disable once IdentifierTypo
	using wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, memory::StlAllocator<wchar_t>>;
}
