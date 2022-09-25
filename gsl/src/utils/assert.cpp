#include <gsl/utils/assert.hpp>
#include <gsl/boost/logger.hpp>

namespace gal::gsl::utils
{
	auto gsl_assert(const bool condition, const accelerate::string_view message, const std::source_location& location) -> void
	{
		if (!condition)
		{
			boost::logger::fatal(
					"[ASSERT FAILED] from [{}/{}/({}-{})]: {}",
					location.file_name(),
					location.function_name(),
					location.line(),
					location.column(),
					message);
		}
	}

	auto gsl_verify(const bool condition, const accelerate::string_view message, const std::source_location& location) -> void { gsl_assert(condition, message, location); }
}
