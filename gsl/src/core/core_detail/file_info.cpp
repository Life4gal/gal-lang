#include <gsl/core/runtime_info.hpp>
#include <algorithm>

namespace gal::gsl::core
{
	file_info::~file_info() = default;

	void file_info::reserve_data_for_profile()
	{
		GSL_ENABLE_PROFILER_DO(
				profile_data_.reserve(
					std::ranges::count(get_source_view(), '\n') + 2);)
	}
}
