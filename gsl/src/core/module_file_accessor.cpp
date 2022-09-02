#include <gsl/core/runtime_info.hpp>

namespace gal::gsl::core
{
	module_file_accessor::module_file_accessor(file_name_view_type pak, const file_accessor_ptr& accessor)
	{
		// todo
		(void)pak;
		(void)accessor;
	}

	file_accessor::file_name_type module_file_accessor::splice_include_path(file_name_view_type filename, file_name_view_type include_filename) const
	{
		if (!valid() || !module_include_getter_)
		{
			return file_accessor::splice_include_path(filename, include_filename);
		}

		// todo
		return {};
	}

	module_info_view module_file_accessor::get_module_info(file_name_view_type request, file_name_view_type from) const
	{
		// todo
		(void)request;
		(void)from;

		return {};
	}

	bool module_file_accessor::is_module_allowed(file_name_view_type module_name, file_name_view_type filename) const
	{
		if (!valid() || !module_allow_checker_)
		{
			return file_accessor::is_module_allowed(module_name, filename);
		}

		// todo
		return false;
	}

}
