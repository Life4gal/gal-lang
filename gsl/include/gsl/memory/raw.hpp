#pragma once

#include <cstddef>

#ifndef GSL_MEMORY_DEBUG
#ifndef _NDEBUG
#include <source_location>
#include <string_view>
#define GSL_MEMORY_DEBUG
#define FIND_LEAK 1
#define GSL_MEMORY_DEBUG_MESSAGE_DECL(message) , std::string_view(message) = "", const std::source_location& location = std::source_location::current()
#define GSL_MEMORY_DEBUG_MESSAGE(message) , const std::string_view(message), const std::source_location& location
#define GSL_MEMORY_DEBUG_MESSAGE_USE(message) , (message).empty() ? location.function_name() : (message).data(), static_cast<int>(location.line())
#endif
#endif

#ifndef GSL_MEMORY_DEBUG
	#define GSL_MEMORY_DEBUG_MESSAGE_DECL(message)
	#define GSL_MEMORY_DEBUG_MESSAGE(message)
	#define GSL_MEMORY_DEBUG_MESSAGE_USE(message)
#endif

namespace gal::gsl::memory
{
	// For any type of object
	[[nodiscard]] auto allocate(std::size_t size GSL_MEMORY_DEBUG_MESSAGE_DECL(message)) -> void*;

	// Optional optimization for allocations that do not contain any pointers within an object
	[[nodiscard]] auto allocate_without_pointer(std::size_t size GSL_MEMORY_DEBUG_MESSAGE_DECL(message)) -> void*;

	// Allocated, but not recycled by us
	[[nodiscard]] auto allocate_without_collect(std::size_t size GSL_MEMORY_DEBUG_MESSAGE_DECL(message)) -> void*;

	// Like allocate_without_pointer
	[[nodiscard]] auto allocate_without_collect_and_pointer(std::size_t size GSL_MEMORY_DEBUG_MESSAGE_DECL(message)) -> void*;

	[[nodiscard]] auto reallocate(void* old_object, std::size_t required_size GSL_MEMORY_DEBUG_MESSAGE_DECL(message)) -> void*;

	// Optional deallocate memory
	auto deallocate(void* data) -> void;
}
