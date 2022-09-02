#include <gsl/core/runtime_info.hpp>

namespace gal::gsl::core
{
	constexpr bool line_info::operator==(const line_info& other) const noexcept
	{
		if (static_cast<bool>(info) != static_cast<bool>(other.info)) { return false; }

		if (info && info->get_filename() != other.info->get_filename()) { return false; }

		return position == other.position;
	}

	constexpr bool line_info::operator<(const line_info& other) const noexcept
	{
		if (info && other.info && info->get_filename() != other.info->get_filename()) { return info->get_filename() < other.info->get_filename(); }

		return position < other.position;
	}

	constexpr bool line_info::include(const line_info& other) const noexcept
	{
		if (info && info != other.info) { return false; }

		if (line() < other.line() || line() >= other.last_line()) { return false; }

		const auto from = line() == other.line() ? other.column() : 0;
		const auto to = line() == other.last_line() ? other.last_column() : std::numeric_limits<size_type>::max();
		return column() >= from && column() < to;
	}
}
