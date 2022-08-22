#pragma once

#include <cstdint>
#include <gsl/core/std/string.hpp>
#include <gsl/core/std/string_view.hpp>
#include <gsl/core/std/memory.hpp>
#include <gsl/core/std/unordered_map.hpp>

#define GSL_ENABLE_PROFILER

#ifdef GSL_ENABLE_PROFILER
#include <gsl/core/std/vector.hpp>
#define GSL_ENABLE_PROFILER_DO(...) __VA_ARGS__
#else
	#define GSL_ENABLE_PROFILER_DO(...)
#endif

namespace gal::gsl::core
{
	enum runtime_type
	{
		none,
		any,
		auto_infer,
		alias,

		void_type,
		bool_type,

		int8_type,
		uint8_type,
		int16_type,
		uint16_type,
		int32_type,
		uint32_type,
		int64_type,
		uint64_type,
		float_type,
		double_type,

		range_type,
		string_type,
		array_type,
		tuple_type,
		table_type,
		enumeration,
		variant_type,

		function_type,
		lambda_type,
	};

	enum class is_reference
	{
		yes,
		no,
	};

	enum class is_const
	{
		yes,
		no,
	};

	// ReSharper disable once IdentifierTypo
	enum class is_xvalue
	{
		yes,
		no,
	};

	template<typename T>
	concept cloneable_t = requires
	{
		std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>;
	};

	class file_info
	{
	public:
		using tab_size_type = std::uint32_t;
		using source_view_type = string_view;

		constexpr static tab_size_type default_tab_size = 4;

	private:
		string filename_;
		// todo: Determine syntax based on indentation?
		std::uint32_t tab_size_{default_tab_size};
		GSL_ENABLE_PROFILER_DO(vector<std::uint64_t> profile_data_;)

	public:
		// Call release_source_data before destructing file_info!
		virtual ~file_info();
		constexpr file_info() = default;
		constexpr file_info(const file_info&) = default;
		constexpr file_info& operator=(const file_info&) = default;
		constexpr file_info(file_info&&) = default;
		constexpr file_info& operator=(file_info&&) = default;

		constexpr void set_filename(const source_view_type filename) { filename_ = filename; }

		void reserve_data_for_profile();

		virtual void release_source_data() noexcept {}
		[[nodiscard]] constexpr virtual source_view_type get_source_view() const noexcept { return {}; }
	};

	using file_info_ptr = unique_ptr<file_info>;

	class text_file_info final : public file_info
	{
	public:
		using value_type = string::value_type;
		using size_type = string::size_type;

		using pointer = string::pointer;
		using const_pointer = string::const_pointer;

	private:
		const_pointer source_;

		size_type length_ : (sizeof(size_type) * 8 - 1);
		size_type owned_ : 1;

	public:
		constexpr text_file_info(const const_pointer source, const size_type length, const bool owned)
			: source_{source},
			  length_{length},
			  owned_{owned} {}

		constexpr text_file_info(const source_view_type source, const bool owned)
			: source_{source.data()},
			  length_{source.size()},
			  owned_{owned} {}

		void release_source_data() noexcept override;

		[[nodiscard]] constexpr source_view_type get_source_view() const noexcept override { return {source_, length_}; }
	};

	struct module_info
	{
		string filename;
		string module_name;
		string import_name;
	};

	struct module_info_view
	{
		string_view filename;
		string_view module_name;
		string_view import_name;
	};

	class file_accessor : public enable_shared_from_this<file_accessor>
	{
	public:
		using file_name_type = string;
		using file_name_view_type = string_view;

		struct file_name_hasher
		{
			using is_transparent = int;

			[[nodiscard]] /* constexpr */ std::size_t operator()(const string& name) const noexcept(std::is_nothrow_invocable_v<std::hash<string>, const string&>) { return std::hash<string>{}(name); }

			[[nodiscard]] /* constexpr */ std::size_t operator()(const file_name_view_type& name) const noexcept(std::is_nothrow_invocable_v<std::hash<file_name_view_type>, const file_name_view_type&>) { return std::hash<file_name_view_type>{}(name); }
		};

		using file_mapping_type = unordered_map<file_name_type, file_info_ptr, file_name_hasher>;

	protected:
		file_mapping_type files_;

		[[nodiscard]] virtual file_info* build_new_file_info(const file_name_view_type name)
		{
			(void)name;
			return nullptr;
		}

	public:
		virtual ~file_accessor();
		file_accessor(const file_accessor&) = default;
		file_accessor& operator=(const file_accessor&) = default;
		file_accessor(file_accessor&&) = default;
		file_accessor& operator=(file_accessor&&) = default;

		// Clears all file data but does not clear file records.
		void release_file_source() noexcept;

		// Clears everything.
		void clear() noexcept
		{
			release_file_source();
			files_.clear();
		}

		// If it does not exist, call build_new_file_info to create a new one.
		// Returns nullptr if it cannot be created.
		[[nodiscard]] file_info* get(file_name_view_type name);
		// If it does not exist, return nullptr directly.
		[[nodiscard]] file_info* get(file_name_view_type name) const;
		file_info* set(file_name_view_type name, file_info_ptr&& file_info);
		[[nodiscard]] file_info_ptr steal(file_name_view_type name);
		[[nodiscard]] virtual bool erase(file_name_view_type name);

		[[nodiscard]] virtual string splice_include_path(file_name_view_type filename, file_name_view_type include_filename) const;
		[[nodiscard]] virtual module_info_view get_module_info(file_name_view_type request, file_name_view_type from) const;
	};
}
