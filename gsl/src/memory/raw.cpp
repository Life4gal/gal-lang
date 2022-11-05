#include <gsl/memory/raw.hpp>

#ifdef GSL_MEMORY_DEBUG
#define GSL_IMPL_MALLOC GC_debug_malloc
#define GSL_IMPL_MALLOC_ATOMIC GC_debug_malloc_atomic
#define GSL_IMPL_MALLOC_UNCOLLECTABLE GC_debug_malloc_uncollectable
#define GSL_IMPL_MALLOC_ATOMIC_UNCOLLECTABLE GC_debug_malloc_atomic_uncollectable
#define GSL_IMPL_REALLOC GC_debug_realloc
#define GSL_IMPL_FREE GC_debug_free
#else
	#define GSL_IMPL_MALLOC GC_malloc
	#define GSL_IMPL_MALLOC_ATOMIC GC_malloc_atomic
	#define GSL_IMPL_MALLOC_UNCOLLECTABLE GC_malloc_uncollectable
	#define GSL_IMPL_MALLOC_ATOMIC_UNCOLLECTABLE GC_malloc_atomic_uncollectable
	#define GSL_IMPL_REALLOC GC_realloc
	#define GSL_IMPL_FREE GC_free
#endif

#include <gc.h>

namespace gal::gsl::memory
{
	auto allocate(const std::size_t size GSL_MEMORY_DEBUG_MESSAGE(message)) -> void* { return GSL_IMPL_MALLOC(size GSL_MEMORY_DEBUG_MESSAGE_USE(message)); }

	auto allocate_without_pointer(const std::size_t size GSL_MEMORY_DEBUG_MESSAGE(message)) -> void* { return GSL_IMPL_MALLOC_ATOMIC(size GSL_MEMORY_DEBUG_MESSAGE_USE(message)); }

	auto allocate_without_collect(const std::size_t size GSL_MEMORY_DEBUG_MESSAGE(message)) -> void* { return GSL_IMPL_MALLOC_UNCOLLECTABLE(size GSL_MEMORY_DEBUG_MESSAGE_USE(message)); }

	auto allocate_without_collect_and_pointer(const std::size_t size GSL_MEMORY_DEBUG_MESSAGE(message)) -> void* { return GSL_IMPL_MALLOC_ATOMIC_UNCOLLECTABLE(size GSL_MEMORY_DEBUG_MESSAGE_USE(message)); }

	auto reallocate(void* old_object, const std::size_t required_size GSL_MEMORY_DEBUG_MESSAGE(message)) -> void* { return GSL_IMPL_REALLOC(old_object, required_size GSL_MEMORY_DEBUG_MESSAGE_USE(message)); }

	auto deallocate(void* data) -> void
	{
		GSL_IMPL_FREE(data);
	}
}
