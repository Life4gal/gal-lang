#include <iostream>
#include <debug_type_register.hpp>
// for test :)
// #define GAL_SCRIPT_LANG_COMPILER_HAS_META 0
#include <gsl/core/type_info.hpp>

#include <eve/wide.hpp>
#include <eve/module/core.hpp>
#include <eve/module/math.hpp>

eve::wide<float> rho(const eve::wide<float> x, eve::wide<float> y) { return eve::sqrt(x * x + y * y); }

eve::wide<float> theta(eve::wide<float> x, eve::wide<float> y) { return eve::atan2(y, x); }

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

	eve::wide<float> x1{4};
	eve::wide<float> y1{[](auto i, auto) { return 1.5f * (i + 1); }};

	std::cout << x1 << " " << y1 << " => " << rho(x1, y1) << "\n";

	float data[] = {1.5f, 3, 4.5f, 6, 7.5f, 9, 10.5f, 12, 13.5, 15, 16.5, 18, 19.5, 21, 22.5, 24};
	eve::wide<float> y2{&data[0]};

	std::cout << x1 << " " << y2 << " => " << theta(x1, y2) << "\n";
}
