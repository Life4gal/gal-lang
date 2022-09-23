#include <gsl/core/file_accessor.hpp>
#include <gsl/core/value.hpp>
#include <gsl/type/raw_string.hpp>
#include <algorithm>
#include <ranges>

namespace gal::gsl::core
{
	FileAccessor::~FileAccessor() = default;

	auto FileAccessor::release_file_source() noexcept -> void
	{
		std::ranges::for_each(
				files_ | std::views::values,
				[](auto& file_info_handler) { file_info_handler->release_source_data(); });
	}

	auto FileAccessor::operator[](const source_view_type name) -> FileInfoRawHandler
	{
		auto file = std::as_const(*this)[name];

		if (file) { return file; }

		file = build_new_file_info(name);
		if (file) { file->reserve_data_for_profile(); }

		return file;
	}

	auto FileAccessor::operator[](const source_view_type name) const -> FileInfoRawHandler
	{
		if (const auto it = files_.find(name);
			it != files_.end()) { return it->second.get(); }

		return nullptr;
	}

	auto FileAccessor::replace(const source_view_type name, FileInfoHandler&& file_info) -> FileInfoRawHandler
	{
		const auto it = files_.find(name);
		if (it != files_.end())
		{
			// todo: replace?
			files_.erase(it);
		}

		const auto dest = files_.emplace_hint(it, name, std::move(file_info));
		dest->second->set_filename(dest->first);
		return dest->second.get();
	}

	auto FileAccessor::steal(source_view_type name) -> FileInfoHandler { return std::move(files_.extract(name).mapped()); }

	auto FileAccessor::erase(source_view_type name) -> bool { return files_.erase(name); }

	auto FileAccessor::combine_include_path(source_view_type filename, source_view_type include_filename) -> FileAccessor::source_type
	{
		// todo: filename format?
		if (const auto p = filename.find_last_of("\\/");
			p != source_view_type::npos)
		{
			source_type result{};
			result.reserve(p + 1 + include_filename.size());
			result.append(filename.substr(0, p + 1));
			result.append(include_filename);
			return result;
		}

		return source_type{include_filename};
	}

	auto FileAccessor::get_module_info(source_view_type request, source_view_type from) const -> module_info
	{
		// todo
		(void)request;
		(void)from;
		return {};
	}

	ModuleFileAccessor::ModuleFileAccessor(source_view_type pak, const FileAccessorHandler& accessor)
	{
		(void)pak;
		(void)accessor;
		// todo
	}

	auto ModuleFileAccessor::combine_include_path(const source_view_type filename, const source_view_type include_filename) -> source_type
	{
		if (!this->operator bool() || !module_include_getter_)
		{
			// fallback
			return FileAccessor::combine_include_path(filename, include_filename);
		}

		// watch out the order!
		Value args[2]{
				ValueCaster<type::raw_string>::from(include_filename.data()),
				ValueCaster<type::raw_string>::from(filename.data())};

		// todo
		(void)args;
		return {};
	}

	auto ModuleFileAccessor::get_module_info(source_view_type request, source_view_type from) const -> module_info
	{
		if (!this->operator bool())
		{
			// fallback
			return FileAccessor::get_module_info(request, from);
		}

		// watch out the order!
		Value args[2]{
				ValueCaster<type::raw_string>::from(request.data()),
				ValueCaster<type::raw_string>::from(from.data())};

		// todo
		(void)args;
		return {};
	}

	auto ModuleFileAccessor::is_module_allowed(const source_view_type module_name, const source_view_type filename) const -> bool
	{
		if (!this->operator bool() || !module_allow_checker_)
		{
			// fallback
			return FileAccessor::is_module_allowed(module_name, filename);
		}

		// watch out the order!
		Value args[2]{
				ValueCaster<type::raw_string>::from(module_name.data()),
				ValueCaster<type::raw_string>::from(filename.data())};

		// todo
		(void)args;
		return {};
	}
}
