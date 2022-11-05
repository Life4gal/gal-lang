#pragma once

#include <gsl/backend/ast.hpp>

namespace gal::gsl::frontend
{
	[[nodiscard]] auto parse_file(string::string_view filename) -> ast::module_type;
}
