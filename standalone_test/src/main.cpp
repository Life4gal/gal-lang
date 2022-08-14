#include <iostream>
#include <debug_type_register.hpp>
// for test :)
// #define GAL_SCRIPT_LANG_COMPILER_HAS_META 0
#include <gsl/core/type_info.hpp>

int main()
{
	std::cout << "Hello gal script lang!"
			// If the compiler is circle then the compiler name we get will be clang :)
			<< "\nCompiler Name: " << GAL_SCRIPT_LANG_COMPILER_NAME
			<< "\nCompiler Version: " << GAL_SCRIPT_LANG_COMPILER_VERSION
			<< "\nCompiler Has META: " << GAL_SCRIPT_LANG_COMPILER_HAS_META
			<< "\nGSL Version: " << GAL_SCRIPT_LANG_VERSION
			<< '\n';

	// test
	const auto type = gal::gsl::core::make_type_info<int>();
	std::cout << type.id << " --> " << debug_type_register::debug_type_names[type.id] << '\n';
}
