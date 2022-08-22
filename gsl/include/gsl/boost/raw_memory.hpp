#pragma once

#include <cstddef>
#include <malloc.h>
#include <gsl/core/macro.hpp>
#include <gsl/boost/logger.hpp>

namespace gal::gsl::boost
{
	struct default_allocator
	{
		[[nodiscard]] static void* alloc(const std::size_t size)
		{
			#ifdef GAL_SCRIPT_LANG_COMPILER_MSVC
			return _aligned_malloc(size, sizeof(std::max_align_t));
			#else
			void* result;
			if (posix_memalign(&result, sizeof(std::max_align_t), size))
			{
				logger::fatal(
						"When trying to allocate memory of size {} (aligned to {}) posix_memalign returned nullptr",
						size,
						sizeof(std::max_align_t)
						);
				GSL_UNREACHABLE();
				return nullptr;
			}
			return result;
			#endif
		}

		static void dealloc(void* ptr)
		{
			#ifdef GAL_SCRIPT_LANG_COMPILER_MSVC
			_aligned_free(ptr);
			#else
			free(ptr);
			#endif
		}

		static std::size_t memory_size(void* ptr)
		{
			#ifdef GAL_SCRIPT_LANG_COMPILER_MSVC
			return _aligned_msize(ptr, sizeof(std::max_align_t), 0);
			#else
			return malloc_usable_size(ptr);
			#endif
		}
	};
}
