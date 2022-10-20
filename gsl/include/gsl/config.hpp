#pragma once

#include <cstdint>
#include <bit>
#include <type_traits>
#include <gsl/accelerate/memory.hpp>

namespace gal::gsl
{
	namespace debug
	{
		auto freeze_here() -> void;
	}

	using heap_data_type = char*;
	using heap_data_size_type = std::uint32_t;

	template<std::size_t Bits>
		requires((Bits & (Bits - 1)) == 0)
	class HeapAllocationAlignedPolicy
	{
	public:
		using size_type = heap_data_size_type;
		using data_type = heap_data_type;

		constexpr static size_type aligned_bits = Bits;
		// for descriptor
		constexpr static size_type aligned_bits_mask = aligned_bits - 1;
		constexpr static size_type aligned_bits_offset = std::countr_zero(aligned_bits);

		/**
		 * \brief Get the smallest (aligned) size
		 * \param size required size
		 * \return the smallest (aligned) size
		 */
		[[nodiscard]] constexpr static auto get_fit_aligned_size(const size_type size) noexcept -> size_type { return (size + aligned_bits_mask) & ~aligned_bits_mask; }
	};

	/**
	 * \brief The allocation size is less than (or equal to) this size to be considered as a "small" allocations
	 */
	constexpr heap_data_size_type heap_small_allocation_threshold = 1 << 8;
	/**
	 * \brief Use "sign bit" as the mark of gc
	 */
	constexpr heap_data_size_type heap_gc_mask = static_cast<heap_data_size_type>(std::numeric_limits<std::make_signed_t<heap_data_size_type>>::max()) + 1;

	#define GSL_ALLOCATIONS_TRACK
	#define GSL_ALLOCATIONS_SANITIZER

	namespace heap
	{
		using data_type = void*;
		using size_type = heap_data_size_type;

		[[nodiscard]] auto allocate(size_type size) -> data_type;

		auto deallocate(data_type data) -> void;

		[[nodiscard]] auto allocated_size(data_type data) -> size_type;

		template<typename T, typename... Args>
		[[nodiscard]] auto new_object(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) -> T*
		{
			T* data = static_cast<T*>(allocate(sizeof(T)));
			std::construct_at(data, std::forward<Args>(args)...);
			return data;
		}

		template<typename T>
		auto delete_object(T*& object) noexcept(std::is_nothrow_destructible_v<T>) -> void
		{
			std::destroy_at(object);
			object = nullptr;
		}
	}
}
