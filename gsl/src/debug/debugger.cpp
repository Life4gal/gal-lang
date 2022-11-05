#include <gsl/debug/debugger.hpp>

#include <gsl/misc/macro.hpp>
#include <csignal>

namespace gal::gsl::debug
{
	auto break_here() -> void
	{
		#ifdef GSL_MSVC
		__debugbreak();
		#else
		raise(SIGTRAP);
		#endif
	}
}
