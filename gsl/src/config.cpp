#include <gsl/config.hpp>
#include <gsl/core/macro.hpp>
#include <gsl/utils/assert.hpp>
#include <cstddef>
#include <malloc.h>
#include <csignal>

namespace gal::gsl
{
	namespace debug
	{
		auto freeze_here() -> void
		{
			#ifdef GSL_MSVC
			__debugbreak();
			#else
			raise(SIGTRAP);
			#endif
		}
	}

	namespace heap
	{
		auto allocate(size_type size) -> data_type
		{
			#ifdef GSL_MSVC
			return static_cast<data_type>(_aligned_malloc(size, alignof(std::max_align_t)));
			#else
			void* data = nullptr;
			if (posix_memalign(&data, alignof(std::max_align_t), size))
			{
				debug::freeze_here();
				gsl_assert(false, "posix_memalign returned nullptr");
				return nullptr;
			}
			return static_cast<data_type>(data);
			#endif
		}

		auto deallocate(data_type data) -> void
		{
			#ifdef GSL_MSVC
			_aligned_free(data);
			#else
			free(data);
			#endif
		}

		auto allocated_size(data_type data) -> size_type
		{
			#ifdef GSL_MSVC
			return static_cast<size_type>(_aligned_msize(data, alignof(std::max_align_t), 0));
			#else
			return static_cast<size_type>(malloc_usable_size(data));
			#endif
		}
	}
}
