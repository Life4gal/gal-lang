#pragma once

#ifndef GSL_NO_ASSERT
	#ifdef _NDEBUG
		#define GSL_NO_ASSERT
	#endif
#endif

#ifndef GSL_NO_ASSERT
#include <gsl/string/string_view.hpp>
#include <source_location>

auto gsl_assert(bool condition, gal::gsl::string::string_view message = "", const std::source_location& location = std::source_location::current()) -> void;
auto gsl_verify(bool condition, gal::gsl::string::string_view message = "", const std::source_location& location = std::source_location::current()) -> void;
auto gsl_trap(gal::gsl::string::string_view message = "", const std::source_location& location = std::source_location::current()) -> void;

#else
	#define gsl_assert(condition, message, location)
	#define gsl_verify(condition, message, location)                  \
		do {                                                          \
			[[maybe_used]] const auto _gsl_verify_result = condition; \
		} while (0);
	#define gsl_trap(message, location)
#endif
