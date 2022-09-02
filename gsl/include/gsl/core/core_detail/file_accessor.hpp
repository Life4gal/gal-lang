#pragma once

#include <gsl/accelerate/memory.hpp>
#include <gsl/accelerate/unordered_map.hpp>

namespace gal::gsl::core
{
	struct module_info
	{
		accelerate::string filename;
		accelerate::string module_name;
		accelerate::string import_name;
	};

	struct module_info_view
	{
		accelerate::string_view filename;
		accelerate::string_view module_name;
		accelerate::string_view import_name;
	};

	class file_accessor : public accelerate::enable_shared_from_this<file_accessor>
	{
	public:
		using file_name_type = file_info::source_type;
		using file_name_view_type = file_info::source_view_type;

		struct file_name_hasher
		{
			using is_transparent = int;

			[[nodiscard]] /* constexpr */ std::size_t operator()(const file_name_type& name) const noexcept(std::is_nothrow_invocable_v<std::hash<file_name_type>, const file_name_type&>) { return std::hash<file_name_type>{}(name); }

			[[nodiscard]] /* constexpr */ std::size_t operator()(const file_name_view_type& name) const noexcept(std::is_nothrow_invocable_v<std::hash<file_name_view_type>, const file_name_view_type&>) { return std::hash<file_name_view_type>{}(name); }
		};

		using file_mapping_type = accelerate::unordered_map<file_name_type, file_info_ptr, file_name_hasher>;

	protected:
		file_mapping_type files_;

		[[nodiscard]] virtual file_info* build_new_file_info(const file_name_view_type name)
		{
			(void)name;
			return nullptr;
		}

	public:
		file_accessor() = default;
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

		[[nodiscard]] virtual file_name_type splice_include_path(file_name_view_type filename, file_name_view_type include_filename) const;
		[[nodiscard]] virtual module_info_view get_module_info(file_name_view_type request, file_name_view_type from) const;

		[[nodiscard]] virtual bool is_module_allowed(const file_name_view_type module_name, const file_name_view_type filename) const
		{
			(void)module_name;
			(void)filename;
			return true;
		}
	};

	using file_accessor_ptr = accelerate::shared_ptr<file_accessor>;

	template<>
	struct is_cloneable<file_accessor> : std::false_type { };
}
