#pragma once

#ifdef GSL_FMT_NOT_SUPPORT
	#include <fmt/format.h>
#else
	#include <format>
#endif

namespace gal::gsl::boost
{
#ifdef GSL_FMT_NOT_SUPPORT
	#include <fmt/format.h>
	namespace format = fmt;
#else
	#include <format>
	namespace format = std;
#endif
}
