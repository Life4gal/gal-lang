#pragma once

#include <gsl/accelerate/memory.hpp>
#include <gsl/accelerate/string.hpp>
#include <gsl/accelerate/string_view.hpp>

#ifdef GSL_ENABLE_PROFILER
#include <gsl/accelerate/vector.hpp>
#define GSL_ENABLE_PROFILER_DO(...) __VA_ARGS__
#else
	#define GSL_ENABLE_PROFILER_DO(...)
#endif

namespace gal::gsl::core
{
	class file_info
	{
	public:
		using tab_size_type = std::uint32_t;
		using source_type = accelerate::string;
		using source_view_type = accelerate::string_view;

		constexpr static tab_size_type default_tab_size = 4;

	private:
		accelerate::string filename_;
		// todo: Determine syntax based on indentation?
		tab_size_type tab_size_{default_tab_size};
		GSL_ENABLE_PROFILER_DO(accelerate::vector<std::uint64_t> profile_data_;)

	public:
		// Call release_source_data before destructing file_info!
		virtual ~file_info();
		file_info() = default;

		explicit file_info(const source_view_type filename, const tab_size_type tab_size = default_tab_size)
			: filename_{filename},
			  tab_size_{tab_size} {}

		file_info(const file_info&) = default;
		file_info& operator=(const file_info&) = default;
		file_info(file_info&&) = default;
		file_info& operator=(file_info&&) = default;

		[[nodiscard]] constexpr source_view_type get_filename() const { return filename_; }
		constexpr void set_filename(const source_view_type filename) { filename_ = filename; }
		[[nodiscard]] constexpr tab_size_type get_tab_size() const { return tab_size_; }
		constexpr void set_tab_size(const tab_size_type new_size) { tab_size_ = new_size; }

		void reserve_data_for_profile();

		virtual void release_source_data() noexcept {}
		[[nodiscard]] constexpr virtual source_view_type get_source_view() const noexcept { return {}; }
	};

	using file_info_ptr = accelerate::unique_ptr<file_info>;
}
