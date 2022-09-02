#pragma once

#include <utility>

namespace gal::gsl::core
{
	class file_info;

	struct line_info
	{
		using size_type = int;
		using position_type = std::pair<size_type, size_type>;

		constexpr static size_type invalid_size = static_cast<size_type>(-1);
		constexpr static position_type invalid_position = position_type{invalid_size, invalid_size};

		static line_info dummy_line_info;

		file_info* info{nullptr};
		// line / column
		position_type position{invalid_position};
		// last_line / last_column
		position_type last_position{invalid_position};

		constexpr line_info() = default;

		constexpr line_info(file_info* info, size_type line, size_type column, size_type last_line, size_type last_column)
			: info{info},
			  position{line, column},
			  last_position{last_line, last_column} {}

		[[nodiscard]] constexpr size_type line() const noexcept { return position.first; }

		[[nodiscard]] constexpr size_type column() const noexcept { return position.second; }

		[[nodiscard]] constexpr size_type last_line() const noexcept { return last_position.first; }

		[[nodiscard]] constexpr size_type last_column() const noexcept { return last_position.second; }

		[[nodiscard]] constexpr bool operator==(const line_info& other) const noexcept;

		[[nodiscard]] constexpr bool operator<(const line_info& other) const noexcept;

		[[nodiscard]] constexpr bool include(const line_info& other) const noexcept;

		[[nodiscard]] constexpr bool empty() const noexcept { return position == position_type{} && last_position == position_type{}; }
	};
}
