#include <iostream>
#include <gsl/memory/allocator.hpp>
#include <span>
#include <gsl/frontend/parse.hpp>

auto main() -> int
{
	std::cout << "Hello gal script lang!"
			<< "\nCompiler Name: " << GAL_SCRIPT_LANG_COMPILER_NAME
			<< "\nCompiler Version: " << GAL_SCRIPT_LANG_COMPILER_VERSION
			<< "\nGSL Version: " << GAL_SCRIPT_LANG_VERSION
			<< '\n';

	gal::gsl::memory::AnyAllocator<int> a;

	auto* arr = static_cast<int*>(a.allocate(24 * sizeof(int)));

	for (size_t i = 0; i < 24; ++i) { arr[i] = static_cast<int>(i * 42); }

	for (const auto i: std::span{arr, 24}) { std::cout << i << '\t'; }

	std::cout << "\n\n" << std::boolalpha;

	try
	{
		if (const auto mod = gal::gsl::frontend::parse_file("test.txt"); 
			!mod) { std::cout << "module '" << mod->get_name() << "' pass failed...\n"; }
		else { std::cout << "module '" << mod->get_name() << "' pass done...\n"; }
	}
	catch (const std::exception& e) { std::cout << "parse failed: " << e.what() << '\n'; }
}
