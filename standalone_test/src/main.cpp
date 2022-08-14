#include <iostream>

int main()
{
	std::cout << "Hello gal script lang!"
			// If the compiler is circle then the compiler name we get will be clang :)
			<< "\nCompiler Name: " << GAL_SCRIPT_LANG_COMPILER_NAME
			<< "\nCompiler Version: " << GAL_SCRIPT_LANG_COMPILER_VERSION
			<< "\nCompiler Has META: " << GAL_SCRIPT_LANG_COMPILER_HAS_META
			<< "\nGSL Version: " << GAL_SCRIPT_LANG_VERSION
			<< '\n';
}
