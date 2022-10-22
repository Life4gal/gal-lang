#pragma once

#include <string>
#include <gsl/memory/allocator.hpp>

namespace gal::gsl::string
{
	using string = std::basic_string<char, std::char_traits<char>, memory::StlAllocator<char>>;
}
