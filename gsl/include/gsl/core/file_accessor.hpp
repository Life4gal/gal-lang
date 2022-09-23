#pragma once

#include <gsl/core/file.hpp>
#include <gsl/accelerate/unordered_map.hpp>

#include "file_accessor.hpp"

namespace gal::gsl::core
{
	struct module_info;
	class FileAccessor;
	using FileAccessorRawHandler = FileAccessor*;
	using FileAccessorRawObserver = const FileAccessor*;
	using FileAccessorHandler = accelerate::shared_ptr<FileAccessor>;
	class ModuleFileAccessor;

	struct module_info
	{
		FileInfo::source_type filename;
		FileInfo::source_type module_name;
		FileInfo::source_type import_name;
	};

	class FileAccessor : public accelerate::enable_shared_from_this<FileAccessor>
	{
	public:
		using source_type = FileInfo::source_type;
		using source_view_type = FileInfo::source_view_type;

		struct file_name_hasher
		{
			using is_transparent = int;

			[[nodiscard]] /* constexpr */ auto operator()(const source_type& name) const noexcept(std::is_nothrow_invocable_v<std::hash<source_type>, const source_type&>) -> std::size_t { return std::hash<source_type>{}(name); }

			[[nodiscard]] /* constexpr */ auto operator()(const source_view_type& name) const noexcept(std::is_nothrow_invocable_v<std::hash<source_view_type>, const source_view_type&>) -> std::size_t { return std::hash<source_view_type>{}(name); }
		};

		using file_mapping_type = accelerate::unordered_map<source_type, FileInfoHandler, file_name_hasher>;

	protected:
		file_mapping_type files_;

		[[nodiscard]] virtual auto build_new_file_info(const source_view_type name) -> FileInfoRawHandler
		{
			(void)name;
			return nullptr;
		}

	public:
		FileAccessor() = default;
		virtual ~FileAccessor();
		FileAccessor(const FileAccessor&) = delete;
		auto operator=(const FileAccessor&) -> FileAccessor& = delete;
		FileAccessor(FileAccessor&&) = default;
		auto operator=(FileAccessor&&) -> FileAccessor& = default;

		// Clears all file data but does not clear file records.
		auto release_file_source() noexcept -> void;

		// Clears everything.
		auto clear() noexcept -> void
		{
			release_file_source();
			files_.clear();
		}

		// If it does not exist, call build_new_file_info to create a new one.
		// Returns nullptr if it cannot be created.
		[[nodiscard]] auto operator[](source_view_type name) -> FileInfoRawHandler;
		// If it does not exist, return nullptr directly.
		[[nodiscard]] auto operator[](source_view_type name) const -> FileInfoRawHandler;

		[[nodiscard]] auto replace(source_view_type name, FileInfoHandler&& file_info) -> FileInfoRawHandler;
		[[nodiscard]] auto steal(source_view_type name) -> FileInfoHandler;

		[[nodiscard]] virtual auto erase(source_view_type name) -> bool;
		[[nodiscard]] virtual auto combine_include_path(source_view_type filename, source_view_type include_filename) -> source_type;
		[[nodiscard]] virtual auto get_module_info(source_view_type request, source_view_type from) const -> module_info;

		[[nodiscard]] virtual auto is_module_allowed(const source_view_type module_name, const source_view_type filename) const -> bool
		{
			(void)module_name;
			(void)filename;
			return true;
		}
	};

	class ModuleContext;
	class ModuleFunction;

	class ModuleFileAccessor final : public FileAccessor
	{
	public:
		using context_type = accelerate::unique_ptr<ModuleContext>;
		using function_type = ModuleFunction*;

	private:
		context_type context_{nullptr};
		function_type module_getter_{nullptr};
		function_type module_include_getter_{nullptr};
		function_type module_allow_checker_{nullptr};

	public:
		ModuleFileAccessor() = default;
		ModuleFileAccessor(source_view_type pak, const FileAccessorHandler& accessor);

		[[nodiscard]] constexpr explicit operator bool() const noexcept
		{
			return context_ && module_getter_;
		}

		auto combine_include_path(source_view_type filename, source_view_type include_filename) -> source_type override;
		auto get_module_info(source_view_type request, source_view_type from) const -> module_info override;
		auto is_module_allowed(source_view_type module_name, source_view_type filename) const -> bool override;
	};
}
