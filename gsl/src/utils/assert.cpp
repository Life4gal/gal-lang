#include <gsl/utils/assert.hpp>

#include <gsl/config.hpp>
#include <gsl/boost/logger.hpp>

namespace
{
	auto gsl_do_assert(const bool condition, const gal::gsl::accelerate::string_view message, const std::source_location& location) -> void
	{
		if (!condition)
		{
			gal::gsl::boost::logger::info(
					"[ASSERT FAILED] from [{}/{}/({}-{})]: {}",
					location.file_name(),
					location.function_name(),
					location.line(),
					location.column(),
					message);
			gal::gsl::debug::freeze_here();
		}
	}
}

auto gsl_assert(const bool condition, const gal::gsl::accelerate::string_view message, const std::source_location& location) -> void { gsl_do_assert(condition, message, location); }

auto gsl_verify(const bool condition, const gal::gsl::accelerate::string_view message, const std::source_location& location) -> void { gsl_do_assert(condition, message, location); }

auto gsl_trap(const gal::gsl::accelerate::string_view message, const std::source_location& location) -> void { gsl_do_assert(false, message, location); }
