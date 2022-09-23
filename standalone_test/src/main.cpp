#include <iostream>
#include <gsl/core/file.hpp>
#include <gsl/core/file_accessor.hpp>
#include <gsl/core/value.hpp>
#include <gsl/type/floating_point.hpp>
#include <gsl/type/integer.hpp>
#include <gsl/type/wide.hpp>
#include <gsl/core/file.hpp>
#include <gsl/type/raw_string.hpp>
#include <gsl/type/boolean.hpp>
#include <gsl/type/type.hpp>


int main()
{
	std::cout << "Hello gal script lang!"
			<< "\nCompiler Name: " << GAL_SCRIPT_LANG_COMPILER_NAME
			<< "\nCompiler Version: " << GAL_SCRIPT_LANG_COMPILER_VERSION
			<< "\nGSL Version: " << GAL_SCRIPT_LANG_VERSION
			<< '\n';
}
