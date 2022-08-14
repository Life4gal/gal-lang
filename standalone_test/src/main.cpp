#include <iostream>

int main()
{
	std::cout << "Hello gal script lang!"
			// If the compiler is circle then the compiler name we get will be clang :)
			<< "\nCompiler Name: " << GAL_SCRIPT_LANG_COMPILER_NAME
			<< "\nCompiler Version: " << GAL_SCRIPT_LANG_COMPILER_VERSION
			<< "\nGSL Version: " << GAL_SCRIPT_LANG_VERSION
			<< '\n';
}
