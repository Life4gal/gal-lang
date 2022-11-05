#pragma once

#include <memory>
#include <gsl/memory/allocator.hpp>

namespace gal::gsl::memory
{
	namespace memory_detail
	{
		template<typename T>
		struct default_deleter
		{
			using allocator_type = StlAllocator<T>;
			using allocator_traits_type = std::allocator_traits<allocator_type>;

			static_assert(sizeof(allocator_type) == 0);

			constexpr auto operator()(typename allocator_traits_type::pointer pointer) const noexcept -> void
			{
				allocator_traits_type::destroy(allocator_type{}, pointer);
				allocator_traits_type::deallocate(allocator_type{}, pointer, 1);
			}
		};
	}

	template<typename T>
	using shared_ptr = std::shared_ptr<T>;

	template<typename T>
	using enable_shared_from_this = std::enable_shared_from_this<T>;

	template<typename T, typename... Args>
	[[nodiscard]] constexpr auto make_shared(Args&&... args) -> shared_ptr<T> { return std::allocate_shared<T>(StlAllocator<T>{}, std::forward<Args>(args)...); }

	template<typename T>
	using unique_ptr = std::unique_ptr<T, memory_detail::default_deleter<T>>;

	template<typename T, typename... Args>
	[[nodiscard]] constexpr auto make_unique(Args&&... args) -> unique_ptr<T>
	{
		using allocator_type = typename memory_detail::default_deleter<T>::allocator_type;
		using allocator_traits_type = typename memory_detail::default_deleter<T>::allocator_traits_type;

		auto holder = unique_ptr<T>{allocator_traits_type::allocate(allocator_type{}, 1)};
		allocator_traits_type::construct(allocator_type{}, holder.get(), std::forward<Args>(args)...);

		return unique_ptr<T>{holder.release()};
	}
}
