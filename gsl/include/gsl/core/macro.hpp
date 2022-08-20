#pragma once

#ifdef GAL_SCRIPT_LANG_COMPILER_MSVC
	#define GSL_UNREACHABLE() __assume(0)
	#define GSL_DEBUG_TRAP() __debugbreak()
	#define GSL_IMPORTED_SYMBOL __declspec(dllimport)
	#define GSL_EXPORTED_SYMBOL __declspec(dllexport)
	#define GSL_LOCAL_SYMBOL

	#define GSL_DISABLE_WARNING_PUSH __pragma(warning(push))
	#define GSL_DISABLE_WARNING_POP __pragma(warning(pop))
	#define GSL_DISABLE_WARNING(warningNumber) __pragma(warning(disable \
															: warningNumber))
#else
	#define GSL_UNREACHABLE() __builtin_unreachable()
	#define GSL_DEBUG_TRAP() __builtin_trap()
	#define GSL_IMPORTED_SYMBOL __attribute__((visibility("default")))
	#define GSL_EXPORTED_SYMBOL __attribute__((visibility("default")))
	#define GSL_LOCAL_SYMBOL __attribute__((visibility("hidden")))

	#define GSL_DISABLE_WARNING_PUSH _Pragma(GCC diagnostic push)
	#define GSL_DISABLE_WARNING_POP _Pragma(GCC diagnostic pop)
	#define GSL_DISABLE_WARNING(warningName) _Pragma(GCC diagnostic ignored #warningName)
#endif
