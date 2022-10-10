#pragma once

#include <cstdint>
#include <memory>
#include <type_traits>

namespace gal::gsl
{
	namespace debug
	{
		auto freeze_here() -> void;
	}

	template<typename SizeType, typename DataType>
		requires std::is_integral_v<SizeType> && std::is_pointer_v<DataType>
	class HeapSmallAllocationPolicy
	{
	public:
		using size_type = SizeType;
		using data_type = DataType;

		constexpr static size_type size_type_size = sizeof(size_type);
		constexpr static size_type size_type_bit_size = size_type_size * 8;
		constexpr static size_type size_type_mask = size_type_bit_size - 1;
		constexpr static size_type size_type_offset = size_type_size + 1;

		[[nodiscard]] constexpr static auto get_fit_capacity(const size_type capacity) noexcept -> size_type { return (capacity + size_type_mask) & ~size_type_mask; }

		[[nodiscard]] constexpr static auto get_descriptor_count(const size_type fit_capacity) noexcept -> size_type { return fit_capacity / 8; }

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
			std::ptrdiff_t test_index = (this_data - root) / size_per_element;
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

	using heap_data_type = char*;
	using heap_data_size_type = std::uint32_t;
	using heap_small_allocation_policy = HeapSmallAllocationPolicy<heap_data_size_type, heap_data_type>;
	constexpr heap_data_size_type kHeapSmallAllocationThreshold = 1 << 8;

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
