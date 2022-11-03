#pragma once

#include <unordered_map>
#include <gsl/memory/allocator.hpp>

namespace gal::gsl::container
{
	namespace unordered_map_detail
	{
		template<typename T>
		using hasher = std::hash<T>;

		template<typename T, bool>
		struct hash_impl : hasher<T> { };

		template<typename T>
		struct hash_impl<T, true> : hasher<T>
		{
			using is_transparent = int;
		};
	}

	template<typename T, bool Transparent>
	struct default_hasher : public unordered_map_detail::hash_impl<T, Transparent> { };

	template<typename T>
		requires std::is_pointer_v<T>
	struct default_hasher<T, true> : public unordered_map_detail::hash_impl<T, true>
	{
		using value_type = typename std::pointer_traits<T>::element_type;

		[[nodiscard]] constexpr auto operator()(value_type* ptr) const
		noexcept(std::is_nothrow_invocable_v<unordered_map_detail::hasher<value_type*>, value_type*>) -> std::size_t { return unordered_map_detail::hasher<value_type*>{}(ptr); }

		[[nodiscard]] constexpr auto operator()(const value_type* ptr) const noexcept(std::is_nothrow_invocable_v<unordered_map_detail::hasher<const value_type*>, const value_type*>) -> std::size_t { return unordered_map_detail::hasher<const value_type*>{}(ptr); }

		template<typename U>
			requires std::is_base_of_v<value_type, typename std::pointer_traits<U>::element_type>
		[[nodiscard]] constexpr auto operator()(typename std::pointer_traits<U>::element_type* ptr) const noexcept(std::is_nothrow_invocable_v<unordered_map_detail::hasher<typename std::pointer_traits<U>::element_type*>, typename std::pointer_traits<U>::element_type*>) -> std::size_t { return unordered_map_detail::hasher<typename std::pointer_traits<U>::element_type*>{}(ptr); }

		template<typename U>
			requires std::is_base_of_v<value_type, typename std::pointer_traits<U>::element_type>
		[[nodiscard]] constexpr auto operator()(const typename std::pointer_traits<U>::element_type* ptr) const noexcept(std::is_nothrow_invocable_v<unordered_map_detail::hasher<const typename std::pointer_traits<U>::element_type*>, const typename std::pointer_traits<U>::element_type*>) -> std::size_t { return unordered_map_detail::hasher<const typename std::pointer_traits<U>::element_type*>{}(ptr); }
	};

	template<
		typename Key,
		typename Value,
		typename Hasher = unordered_map_detail::hasher<std::pair<Key, const Value>>,
		typename KeyComparator = std::equal_to<>>
	using unordered_map = std::unordered_map<
		Key,
		Value,
		Hasher,
		KeyComparator,
		memory::StlAllocator<std::pair<Key, const Value>>
	>;

	template<
		typename Key,
		typename Value,
		typename Hasher = unordered_map_detail::hasher<std::pair<Key, const Value>>,
		typename KeyComparator = std::equal_to<>>
	using unordered_multimap = std::unordered_multimap<
		Key,
		Value,
		Hasher,
		KeyComparator,
		memory::StlAllocator<std::pair<Key, const Value>>>;
}
