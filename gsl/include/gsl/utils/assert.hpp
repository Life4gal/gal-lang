#pragma once

#include <source_location>
#include <gsl/accelerate/string_view.hpp>

#ifndef GSL_NO_ASSERT
#ifdef _NDEBUG
		#define GSL_NO_ASSERT
#endif
#endif

#ifndef GSL_NO_ASSERT
auto gsl_assert(bool condition, gal::gsl::accelerate::string_view message, const std::source_location& location = std::source_location::current()) -> void;
#else
#define gsl_assert(condition, message, location)
#endif

auto gsl_verify(bool condition, gal::gsl::accelerate::string_view message, const std::source_location& location = std::source_location::current()) -> void;
