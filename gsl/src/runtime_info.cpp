#include <gsl/core/runtime_info.hpp>

#include <gsl/boost/raw_memory.hpp>
#include <algorithm>
#include <ranges>

namespace gal::gsl::core
{
	file_info::~file_info() = default;

	void file_info::reserve_data_for_profile()
	{
		GSL_ENABLE_PROFILER_DO(
				profile_data_.reserve(
					std::ranges::count(get_source_view(), '\n') + 2
				);
				)
	}

	void text_file_info::release_source_data() noexcept
	{
		if (owned_ && source_)
		{
			boost::default_allocator::dealloc(const_cast<pointer>(source_));
			source_ = nullptr;
			length_ = 0;
			owned_ = false;
		}
	}

	file_accessor::~file_accessor() = default;

	void file_accessor::release_file_source() noexcept
	{
		std::ranges::for_each(
				files_ | std::views::values,
				[](auto& fip) { fip->release_source_data(); });
	}

	file_info* file_accessor::get(const file_name_view_type name)
	{
		if (const auto it = files_.find(name);
			it != files_.end()) { return it->second.get(); }

		if (const auto fi = build_new_file_info(name);
			fi)
		{
			fi->reserve_data_for_profile();
			return fi;
		}

		return nullptr;
	}

	file_info* file_accessor::get(const file_name_view_type name) const
	{
		if (const auto it = files_.find(name);
			it != files_.end()) { return it->second.get(); }

		return nullptr;
	}

	file_info* file_accessor::set(const file_name_view_type name, file_info_ptr&& file_info)
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

	file_info_ptr file_accessor::steal(const file_name_view_type name)
	{
		if (const auto it = files_.find(name);
			it != files_.end())
		{
			return std::move(it->second);
			// todo: How about key?
		}
		return nullptr;
	}

	bool file_accessor::erase(const file_name_view_type name)
	{
		if (const auto it = files_.find(name);
			it != files_.end())
		{
			files_.erase(it);
			return true;
		}
		return false;
	}

	string file_accessor::splice_include_path(const file_name_view_type filename, const file_name_view_type include_filename) const
	{
		if (const auto p = filename.find_last_of("\\/");
			p != file_name_view_type::npos)
		{
			string result{};
			result.reserve(p + 1 + include_filename.size());
			result.append(filename.substr(0, p + 1));
			result.append(include_filename);
			return result;
		}

		return string{include_filename};
	}

	module_info_view file_accessor::get_module_info(const file_name_view_type request, const file_name_view_type from) const
	{
		// todo
		(void)request;
		(void)from;
		return {};
	}
}
