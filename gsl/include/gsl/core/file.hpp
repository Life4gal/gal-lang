#pragma once

#include <cstdint>
#include <limits>
#include <gsl/accelerate/string.hpp>
#include <gsl/accelerate/string_view.hpp>
#include <gsl/accelerate/memory.hpp>

#include "file.hpp"

// todo
#define GSL_ENABLE_PROFILER

#ifdef GSL_ENABLE_PROFILER
#include <gsl/accelerate/vector.hpp>
#define GSL_ENABLE_PROFILER_DO(...) __VA_ARGS__
#else
	#define GSL_ENABLE_PROFILER_DO(...)
#endif

namespace gal::gsl::core
{
	struct LineInfo;
	class FileInfo;
	using FileInfoRawHandler = FileInfo*;
	using FileInfoRawObserver = const FileInfo*;
	using FileInfoHandler = accelerate::unique_ptr<FileInfo>;
	class TextFileInfo;

	struct LineInfo
	{
		static LineInfo dummy_line_info;

		using size_type = int;
		using position_type = std::pair<size_type, size_type>;

		constexpr static size_type invalid_size = static_cast<size_type>(-1);
		constexpr static position_type invalid_position = position_type{invalid_size, invalid_size};

		FileInfoRawObserver file_info{nullptr};
		// line / column
		position_type position{invalid_position};
		// last_line / last_column
		position_type last_position{invalid_position};

		constexpr LineInfo() = default;

		constexpr LineInfo(const FileInfoRawHandler file, size_type line, size_type column, size_type last_line, size_type last_column)
			: file_info{file},
			position{line, column},
			last_position{last_line, last_column} {}

		[[nodiscard]] constexpr auto line() const noexcept -> size_type { return position.first; }

		[[nodiscard]] constexpr auto column() const noexcept -> size_type { return position.second; }

		[[nodiscard]] constexpr auto last_line() const noexcept -> size_type { return last_position.first; }

		[[nodiscard]] constexpr auto last_column() const noexcept -> size_type { return last_position.second; }

		[[nodiscard]] constexpr auto operator==(const LineInfo& other) const noexcept -> bool;

		[[nodiscard]] constexpr auto operator<(const LineInfo& other) const noexcept -> bool;

		[[nodiscard]] constexpr auto include(const LineInfo& other) const noexcept -> bool;

		[[nodiscard]] constexpr auto empty() const noexcept -> bool { return position == position_type{} && last_position == position_type{}; }
	};

	template<size_t N>
		requires(N < std::tuple_size_v<LineInfo>)
	constexpr auto get(const LineInfo& line_info) noexcept -> LineInfo::size_type
	{
		// line/column/last_line/last_column
		if constexpr (N == 0) { return line_info.line(); }
		else if constexpr (N == 1) { return line_info.column(); }
		else if constexpr (N == 2) { return line_info.last_line(); }
		else if constexpr (N == 3) { return line_info.last_column(); }
		else { std::unreachable(); }
	}

	class FileInfo
	{
	public:
		using tab_size_type = std::uint32_t;
		using source_type = accelerate::string;
		using source_view_type = accelerate::string_view;

		constexpr static tab_size_type default_tab_size = 4;

	private:
		source_type filename_;
		tab_size_type tab_size_{default_tab_size};
		GSL_ENABLE_PROFILER_DO(accelerate::vector<std::uint64_t> profile_data_;)

	public:
		// Call release_source_data before destructing file_info!
		virtual ~FileInfo();
		FileInfo() = default;

		explicit FileInfo(const source_view_type filename, const tab_size_type tab_size = default_tab_size)
			: filename_{filename},
			tab_size_{tab_size} {}

		FileInfo(const FileInfo&) = default;
		auto operator=(const FileInfo&) -> FileInfo& = default;
		FileInfo(FileInfo&&) = default;
		auto operator=(FileInfo&&) -> FileInfo& = default;

		[[nodiscard]] constexpr auto get_filename() const -> source_view_type { return filename_; }
		constexpr auto set_filename(const source_view_type filename) -> void { filename_ = filename; }
		[[nodiscard]] constexpr auto get_tab_size() const -> tab_size_type { return tab_size_; }
		constexpr auto set_tab_size(const tab_size_type new_size) -> void { tab_size_ = new_size; }

		auto reserve_data_for_profile() -> void;

		virtual auto release_source_data() noexcept -> void {}
		[[nodiscard]] constexpr virtual auto get_source_view() const noexcept -> source_view_type { return {}; }
	};

	class TextFileInfo final : public FileInfo
	{
	private:
		source_type source_;

	public:
		explicit TextFileInfo(const source_type& source)
			: source_{source} {}

		explicit TextFileInfo(source_type&& source)
			: source_{std::move(source)} {}

		auto release_source_data() noexcept -> void override { source_.clear(); }
		[[nodiscard]] auto get_source_view() const noexcept -> source_view_type override { return source_; }
	};

	constexpr auto LineInfo::operator==(const LineInfo& other) const noexcept -> bool
	{
		if (static_cast<bool>(file_info) != static_cast<bool>(other.file_info)) { return false; }

		if (file_info && file_info->get_filename() != other.file_info->get_filename()) { return false; }

		return position == other.position;
	}

	constexpr auto LineInfo::operator<(const LineInfo& other) const noexcept -> bool
	{
		if (file_info && other.file_info && file_info->get_filename() != other.file_info->get_filename()) { return file_info->get_filename() < other.file_info->get_filename(); }

		return position < other.position;
	}
}

namespace std
{
	// line/column + last_line/last_column
	template<>
	struct tuple_size<gal::gsl::core::LineInfo> : integral_constant<size_t, 4> {};

	template<size_t N>
	struct tuple_element<N, gal::gsl::core::LineInfo>
	{
		using type = gal::gsl::core::LineInfo::size_type;
	};
}

namespace gal::gsl::core
{
	constexpr auto LineInfo::include(const LineInfo& other) const noexcept -> bool
	{
		if (file_info && file_info != other.file_info) { return false; }

		const auto [this_line, this_column, this_last_line, this_last_column] = *this;
		const auto [other_line, other_column, other_last_line, other_last_column] = other;

		if (this_line < other_line || this_line >= other_last_line) { return false; }

		const auto from = this_line == other_line ? other_column : 0;
		const auto to = this_line == other_last_line ? other_last_column : std::numeric_limits<size_type>::max();
		return this_column >= from && this_column < to;
	}
}
