#include <gsl/core/file.hpp>
#include <algorithm>

namespace gal::gsl::core
{
	FileInfo::~FileInfo() = default;

	auto FileInfo::reserve_data_for_profile() -> void
	{
		GSL_ENABLE_PROFILER_DO(
				profile_data_.reserve(
					std::ranges::count(get_source_view(), '\n') + 2
				)
				);
	}
}
