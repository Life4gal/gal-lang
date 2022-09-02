#pragma once

#include <fmt/format.h>
#include <gsl/accelerate/string.hpp>

#ifndef GAL_SCRIPT_LANG_FIXED_BUFFER_SIZE
#define GAL_SCRIPT_LANG_FIXED_BUFFER_SIZE 1 << 12
#endif

namespace gal::gsl::utils
{
	using fixed_string_writer = fmt::basic_memory_buffer<accelerate::string::value_type, GAL_SCRIPT_LANG_FIXED_BUFFER_SIZE, accelerate::string::allocator_type>;
	using text_string_writer = accelerate::string;
}
