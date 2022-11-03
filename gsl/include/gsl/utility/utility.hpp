#pragma once

#include <utility>
#include <type_traits>

namespace gal::gsl::utility
{
	template<typename T>
	using default_hash_impl = std::hash<T>;

	template<typename String>
	struct string_hasher
	{
		using is_transparent = int;

		using string_type = String;
		using string_view_type = std::basic_string_view<typename string_type::value_type, typename string_type::traits_type>;

		struct do_hash
		{
			[[nodiscard]] constexpr auto operator()(const std::ranges::range auto& container) noexcept -> std::size_t
			{
				// FNV-1a hash. See: http://www.isthe.com/chongo/tech/comp/fnv/
				constexpr std::size_t hash_init{14695981039346656037ull};
				constexpr std::size_t hash_prime{1099511628211ull};

				auto hash = hash_init;

				for (const auto i: container)
				{
					hash ^= i;
					hash *= hash_prime;
				}

				return hash;
			}
		};

		[[nodiscard]] constexpr auto operator()(const string_type& string) const noexcept -> std::size_t { return do_hash{}(string); }

		[[nodiscard]] constexpr auto operator()(const string_view_type& string) const noexcept -> std::size_t { return do_hash{}(string); }
	};

	template<typename T>
		requires std::is_pointer_v<T>
	struct pointer_hasher
	{
		using is_transparent = int;

		using value_type = typename std::pointer_traits<T>::element_type;

		[[nodiscard]] constexpr auto operator()(value_type* ptr) const
		noexcept(std::is_nothrow_invocable_v<default_hash_impl<value_type*>, value_type*>) -> std::size_t { return default_hash_impl<value_type*>{}(ptr); }

		[[nodiscard]] constexpr auto operator()(const value_type* ptr) const noexcept(std::is_nothrow_invocable_v<default_hash_impl<const value_type*>, const value_type*>) -> std::size_t { return default_hash_impl<const value_type*>{}(ptr); }

		template<typename U>
			requires std::is_base_of_v<value_type, typename std::pointer_traits<U>::element_type>
		[[nodiscard]] constexpr auto operator()(typename std::pointer_traits<U>::element_type* ptr) const noexcept(std::is_nothrow_invocable_v<default_hash_impl<typename std::pointer_traits<U>::element_type*>, typename std::pointer_traits<U>::element_type*>) -> std::size_t { return default_hash_impl<typename std::pointer_traits<U>::element_type*>{}(ptr); }

		template<typename U>
			requires std::is_base_of_v<value_type, typename std::pointer_traits<U>::element_type>
		[[nodiscard]] constexpr auto operator()(const typename std::pointer_traits<U>::element_type* ptr) const noexcept(std::is_nothrow_invocable_v<default_hash_impl<const typename std::pointer_traits<U>::element_type*>, const typename std::pointer_traits<U>::element_type*>) -> std::size_t { return default_hash_impl<const typename std::pointer_traits<U>::element_type*>{}(ptr); }
	};
}
