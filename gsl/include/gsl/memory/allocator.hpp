#pragma once

#include <type_traits>
#include <memory>
#include <gsl/memory/raw.hpp>

namespace gal::gsl::memory
{
	template<typename T>
	struct can_allocate_atomic : std::false_type {};

	template<typename T>
		requires std::is_arithmetic_v<T>
	struct can_allocate_atomic<T> : std::true_type { };

	template<typename T>
	constexpr bool can_allocate_atomic_v = can_allocate_atomic<T>::value;

	template<typename T>
	class AnyAllocator
	{
	public:
		using allocator_type = AnyAllocator<T>;
		using trait_type = std::allocator_traits<allocator_type>;

		using value_type = typename trait_type::value_type;

		using pointer = typename trait_type::pointer;
		using const_pointer = typename trait_type::const_pointer;
		using void_pointer = typename trait_type::void_pointer;
		using const_void_pointer = typename trait_type::const_void_pointer;

		using size_type = typename trait_type::size_type;
		using difference_type = typename trait_type::difference_type;

		using propagate_on_container_copy_assignment = typename trait_type::propagate_on_container_copy_assignment;
		using propagate_on_container_move_assignment = typename trait_type::propagate_on_container_move_assignment;
		using propagate_on_container_swap = typename trait_type::propagate_on_container_swap;
		using is_always_equal = typename trait_type::is_always_equal;

		template<typename U>
		using rebind_alloc = typename trait_type::template rebind_alloc<U>;

		template<typename U>
		using rebind_traits = typename trait_type::template rebind_traits<U>;

		constexpr AnyAllocator() noexcept = default;
		constexpr AnyAllocator(const AnyAllocator&) noexcept = default;
		constexpr AnyAllocator(AnyAllocator&&) noexcept = default;
		constexpr auto operator=(const AnyAllocator&) noexcept -> AnyAllocator& = default;
		constexpr auto operator=(AnyAllocator&&) noexcept -> AnyAllocator& = default;
		constexpr ~AnyAllocator() noexcept = default;

		template<typename U>
		constexpr explicit(false) AnyAllocator(const AnyAllocator<U>&) noexcept {}

		[[nodiscard]] constexpr auto allocate(const size_type size) -> pointer
		{
			(void)this;

			static_assert(sizeof(value_type), "value_type must be complete before calling allocate.");

			if constexpr (can_allocate_atomic_v<value_type>) { return static_cast<pointer>(memory::allocate_without_pointer(size)); }
			else { return static_cast<pointer>(memory::allocate(size * sizeof(value_type))); }
		}

		constexpr auto deallocate(const pointer pointer, const size_type size) noexcept -> void
		{
			(void)this;
			(void)size;

			memory::deallocate(pointer);
		}

		[[nodiscard]] friend constexpr auto operator==(const AnyAllocator&, const AnyAllocator&) noexcept -> bool { return true; }
	};

	// Similar to 'AnyAllocator', but the memory deallocation is left to the container using the allocator itself
	template<typename T>
	class StlAllocator
	{
	public:
		using allocator_type = StlAllocator<T>;
		using trait_type = std::allocator_traits<allocator_type>;

		using value_type = typename trait_type::value_type;

		using pointer = typename trait_type::pointer;
		using const_pointer = typename trait_type::const_pointer;
		using void_pointer = typename trait_type::void_pointer;
		using const_void_pointer = typename trait_type::const_void_pointer;

		using size_type = typename trait_type::size_type;
		using difference_type = typename trait_type::difference_type;

		using propagate_on_container_copy_assignment = typename trait_type::propagate_on_container_copy_assignment;
		using propagate_on_container_move_assignment = typename trait_type::propagate_on_container_move_assignment;
		using propagate_on_container_swap = typename trait_type::propagate_on_container_swap;
		using is_always_equal = typename trait_type::is_always_equal;

		template<typename U>
		using rebind_alloc = typename trait_type::template rebind_alloc<U>;

		template<typename U>
		using rebind_traits = typename trait_type::template rebind_traits<U>;

		constexpr StlAllocator() noexcept = default;
		constexpr StlAllocator(const StlAllocator&) noexcept = default;
		constexpr StlAllocator(StlAllocator&&) noexcept = default;
		constexpr auto operator=(const StlAllocator&) noexcept -> StlAllocator& = default;
		constexpr auto operator=(StlAllocator&&) noexcept -> StlAllocator& = default;
		constexpr ~StlAllocator() noexcept = default;

		template<typename U>
		constexpr explicit(false) StlAllocator(const StlAllocator<U>&) noexcept { }

		[[nodiscard]] constexpr auto allocate(const size_type size) -> pointer
		{
			(void)this;

			static_assert(sizeof(value_type), "value_type must be complete before calling allocate.");

			if constexpr (can_allocate_atomic_v<value_type>) { return static_cast<pointer>(memory::allocate_without_collect_and_pointer(size)); }
			else { return static_cast<pointer>(memory::allocate_without_collect(size * sizeof(value_type))); }
		}

		constexpr auto deallocate(const pointer pointer, const size_type size) noexcept -> void
		{
			(void)this;
			(void)size;

			memory::deallocate(pointer);
		}

		[[nodiscard]] friend constexpr auto operator==(const StlAllocator&, const StlAllocator&) noexcept -> bool { return true; }
	};

	class TrivialAllocator
	{
	public:
		using value_type = void;
		using size_type = std::size_t;

		using pointer = void*;
		using const_pointer = const void*;

		template<typename T = value_type>
		[[nodiscard]] auto allocate(const size_type size) const -> pointer
		{
			(void)this;

			if constexpr (std::is_same_v<T, value_type>) { return memory::allocate(size); }
			else
			{
				if constexpr (can_allocate_atomic_v<T>) { return static_cast<pointer>(memory::allocate_without_pointer(size)); }
				else { return static_cast<pointer>(memory::allocate(size * sizeof(T))); }
			}
		}

		auto deallocate(const pointer pointer, const size_type size) const noexcept -> void
		{
			(void)this;
			(void)size;

			memory::deallocate(pointer);
		}
	};
}

template<typename T>
struct std::allocator_traits<gal::gsl::memory::AnyAllocator<T>>
{
	using allocator_type = gal::gsl::memory::AnyAllocator<T>;

	using value_type = T;

	using pointer = value_type*;
	using const_pointer = const value_type*;
	using void_pointer = void*;
	using const_void_pointer = const void*;

	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using propagate_on_container_copy_assignment = true_type;
	using propagate_on_container_move_assignment = true_type;
	using propagate_on_container_swap = true_type;
	using is_always_equal = true_type;

	template<typename U>
	using rebind_alloc = gal::gsl::memory::AnyAllocator<U>;

	template<typename U>
	using rebind_traits = allocator_traits<rebind_alloc<U>>;

	[[nodiscard]] constexpr static auto allocate(allocator_type& allocator, const size_type size) -> pointer { return allocator.allocate(size); }

	constexpr static auto deallocate(allocator_type& allocator, const pointer pointer, const size_type size) noexcept -> void { allocator.deallocate(pointer, size); }

	template<typename... Args>
	constexpr static auto construct([[maybe_unused]] allocator_type& allocator, pointer pointer, Args&&... args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>) -> void { std::construct_at(pointer, std::forward<Args>(args)...); }

	constexpr static auto destroy([[maybe_unused]] allocator_type& allocator, pointer pointer) noexcept(std::is_nothrow_destructible_v<value_type>) -> void { std::destroy_at(pointer); }

	[[nodiscard]] constexpr static auto max_size([[maybe_unused]] const allocator_type& allocator) noexcept -> size_type { return static_cast<size_type>(-1) / sizeof(value_type); }

	[[nodiscard]] constexpr static auto select_on_container_copy_construction(const allocator_type& allocator) noexcept -> allocator_type { return allocator; }
};

template<typename T>
struct std::allocator_traits<gal::gsl::memory::StlAllocator<T>>
{
	using allocator_type = gal::gsl::memory::StlAllocator<T>;

	using value_type = T;

	using pointer = value_type*;
	using const_pointer = const value_type*;
	using void_pointer = void*;
	using const_void_pointer = const void*;

	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using propagate_on_container_copy_assignment = true_type;
	using propagate_on_container_move_assignment = true_type;
	using propagate_on_container_swap = true_type;
	using is_always_equal = true_type;

	template<typename U>
	using rebind_alloc = gal::gsl::memory::StlAllocator<U>;

	template<typename U>
	using rebind_traits = allocator_traits<rebind_alloc<U>>;

	[[nodiscard]] constexpr static auto allocate(allocator_type& allocator, const size_type size) -> pointer { return allocator.allocate(size); }

	constexpr static auto deallocate(allocator_type& allocator, const pointer pointer, const size_type size) noexcept -> void { allocator.deallocate(pointer, size); }

	template<typename... Args>
	constexpr static auto construct([[maybe_unused]] allocator_type& allocator, pointer pointer, Args&&... args) noexcept(std::is_nothrow_constructible_v<value_type, Args...>) -> void { std::construct_at(pointer, std::forward<Args>(args)...); }

	constexpr static auto destroy([[maybe_unused]] allocator_type& allocator, pointer pointer) noexcept(std::is_nothrow_destructible_v<value_type>) -> void { std::destroy_at(pointer); }

	[[nodiscard]] constexpr static auto max_size([[maybe_unused]] const allocator_type& allocator) noexcept -> size_type { return static_cast<size_type>(-1) / sizeof(value_type); }

	[[nodiscard]] constexpr static auto select_on_container_copy_construction(const allocator_type& allocator) noexcept -> allocator_type { return allocator; }
};
