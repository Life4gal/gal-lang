#pragma once

#include <gsl/core/core_detail/type_info.hpp>

namespace gal::gsl::core
{
	class type_info_variable : public type_info { };

	class type_info_local_variable : public type_info { };

	class type_info_struct { };

	class type_info_enum_value { };

	class type_info_enum { };

	class type_info_function { };
}
