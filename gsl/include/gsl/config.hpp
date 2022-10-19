#pragma once

#include <cstdint>
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

	template<typename AlignedType>
		requires std::is_integral_v<AlignedType>
	class HeapSmallAllocationPolicy
	{
	public:
		using size_type = heap_data_size_type;
		using data_type = heap_data_type;

		using aligned_type = AlignedType;

		template<typename NewAlignedSize>
			requires std::is_integral_v<NewAlignedSize>
		using rebind_policy = HeapSmallAllocationPolicy<NewAlignedSize>;

		constexpr static size_type size_type_size = sizeof(aligned_type);
		constexpr static size_type size_type_bit_size = size_type_size * 8;
		constexpr static size_type size_type_mask = size_type_bit_size - 1;
		constexpr static size_type size_type_offset = size_type_size + 1;

		[[nodiscard]] constexpr static auto get_fit_aligned_size(const size_type size) noexcept -> size_type { return (size + size_type_mask) & ~size_type_mask; }

		[[nodiscard]] constexpr static auto get_descriptor_count(const size_type fit_aligned_size) noexcept -> size_type { return fit_aligned_size / 8; }

		struct descriptor_state
		{
			size_type index;
			size_type offset;
			size_type mask;

			[[nodiscard]] constexpr auto operator==(const descriptor_state& other) const noexcept -> bool = default;
		};

		constexpr static descriptor_state bad_descriptor{
				.index = static_cast<size_type>(-1),
				.offset = static_cast<size_type>(-1),
				.mask = static_cast<size_type>(-1)};

		[[nodiscard]] constexpr static auto get_descriptor_state(const data_type root, const size_type capacity, const size_type size_per_element, const data_type this_data) noexcept -> descriptor_state
		{
			const std::ptrdiff_t test_index = (this_data - root) / size_per_element;
			if (test_index < 0 || test_index >= capacity)
			{
				// todo: let it crash
				return bad_descriptor;
			}

			const auto real_index = static_cast<size_type>(test_index);
			const auto offset = real_index & size_type_mask;

			return {.index = real_index >> size_type_offset, .offset = offset, .mask = size_type{1} << offset};
		}
	};

	using heap_small_allocation_policy = HeapSmallAllocationPolicy<std::uint32_t>;
	constexpr heap_data_size_type kHeapSmallAllocationThreshold = 1 << 8;
	constexpr heap_data_size_type kHeapGcMask = std::numeric_limits<std::make_signed_t<heap_data_size_type>>::max() + 1;

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
