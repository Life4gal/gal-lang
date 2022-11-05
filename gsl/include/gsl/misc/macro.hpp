#pragma once

#if defined(GAL_SCRIPT_LANG_COMPILER_MSVC) || defined(_MSC_VER)
	#define GSL_MSVC
	#define GSL_UNREACHABLE() __assume(0)
	#define GSL_DEBUG_TRAP() __debugbreak()
	#define GSL_IMPORTED_SYMBOL __declspec(dllimport)
	#define GSL_EXPORTED_SYMBOL __declspec(dllexport)
	#define GSL_LOCAL_SYMBOL

	#define GSL_DISABLE_WARNING_PUSH __pragma(warning(push))
	#define GSL_DISABLE_WARNING_POP __pragma(warning(pop))
	#define GSL_DISABLE_WARNING(warningNumber) __pragma(warning(disable \
																: warningNumber))
#elif defined(GAL_SCRIPT_LANG_COMPILER_GNU) || defined(__GNUC__)
	#define GSL_GNU
	#define GSL_UNREACHABLE() __builtin_unreachable()
	#define GSL_DEBUG_TRAP() __builtin_trap()
	#define GSL_IMPORTED_SYMBOL __attribute__((visibility("default")))
	#define GSL_EXPORTED_SYMBOL __attribute__((visibility("default")))
	#define GSL_LOCAL_SYMBOL __attribute__((visibility("hidden")))

	#define GSL_DISABLE_WARNING_PUSH _Pragma("GCC diagnostic push")
	#define GSL_DISABLE_WARNING_POP _Pragma("GCC diagnostic pop")

	#define GSL_PRIVATE_DO_PRAGMA(X) _Pragma(#X)
	#define GSL_DISABLE_WARNING(warningName) GSL_PRIVATE_DO_PRAGMA(GCC diagnostic ignored #warningName)
#elif defined(GAL_SCRIPT_LANG_COMPILER_CLANG) || defined(__clang__)
	#define GSL_CLANG
	#define GSL_UNREACHABLE() __builtin_unreachable()
	#define GSL_DEBUG_TRAP() __builtin_trap()
	#define GSL_IMPORTED_SYMBOL __attribute__((visibility("default")))
	#define GSL_EXPORTED_SYMBOL __attribute__((visibility("default")))
	#define GSL_LOCAL_SYMBOL __attribute__((visibility("hidden")))

	#define GSL_DISABLE_WARNING_PUSH _Pragma("Clang diagnostic push")
	#define GSL_DISABLE_WARNING_POP _Pragma("Clang diagnostic pop")

	#define GSL_PRIVATE_DO_PRAGMA(X) _Pragma(#X)
	#define GSL_DISABLE_WARNING(warningName) GSL_PRIVATE_DO_PRAGMA(Clang diagnostic ignored #warningName)
#else
	#define GSL_UNREACHABLE()
	#define GSL_DEBUG_TRAP()
	#define GSL_IMPORTED_SYMBOL
	#define GSL_EXPORTED_SYMBOL
	#define GSL_LOCAL_SYMBOL

	#define GSL_DISABLE_WARNING_PUSH
	#define GSL_DISABLE_WARNING_POP
	#define GSL_DISABLE_WARNING(warningName)
#endif
