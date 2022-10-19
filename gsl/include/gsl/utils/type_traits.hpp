#pragma once

#include <type_traits>

namespace gal::gsl::utils
{
	namespace type_traits_detail
	{
		template<typename DefaultType, typename, template<typename...> typename Operator, typename... Args>
		struct detector : std::false_type
		{
			using type = DefaultType;
		};

		template<typename DefaultType, template<typename...> typename Operator, typename... Args>
		struct detector<DefaultType, std::void_t<Operator<Args...>>, Operator, Args...> : std::true_type
		{
			using type = Operator<Args...>;
		};
	}

	// basic
	struct detect_failed {};

	template<template<typename...> typename Operator, typename... Args>
	using is_detected = type_traits_detail::detector<detect_failed, void, Operator, Args...>;

	template<template<typename...> typename Operator, typename... Args>
	using detected_t = typename type_traits_detail::detector<detect_failed, void, Operator, Args...>::type;

	template<typename Default, template<typename...> typename Operator, typename... Args>
	using detected_or = type_traits_detail::detector<Default, void, Operator, Args...>;

	// tools
	template<template<typename...> typename Operator, typename... Args>
	constexpr bool is_detected_v = is_detected<Operator, Args...>::value;

	template<typename Default, template<typename...> typename Operator, typename... Args>
	using detected_or_t = typename detected_or<Default, Operator, Args...>::type;

	template<typename Expected, template<typename...> typename Operator, typename... Args>
	using is_detected_exact = std::is_same<Expected, detected_t<Operator, Args...>>;

	template<typename Expected, template<typename...> typename Operator, typename... Args>
	constexpr bool is_detected_exact_v = is_detected_exact<Expected, Operator, Args...>::value;

	template<typename To, template<typename...> typename Operator, typename... Args>
	using is_detected_convertible = std::is_convertible<detected_t<Operator, Args...>, To>;

	template<typename To, template<typename...> typename Operator, typename... Args>
	constexpr bool is_detected_convertible_v = is_detected_convertible<To, Operator, Args...>::value;

	namespace type_traits_detail::test_detected
	{
		// test function
		template<typename T>
		using copy_assignable_t = decltype(std::declval<T&>() = std::declval<const T&>());

		struct has_copy_assign// NOLINT(cppcoreguidelines-special-member-functions)
		{
			using copy_assign_return_type = double;

			auto operator=(const has_copy_assign&) -> copy_assign_return_type = default;// NOLINT(misc-unconventional-assign-operator)
		};

		struct no_copy_assign// NOLINT(cppcoreguidelines-special-member-functions)
		{
			auto operator=(const no_copy_assign&) -> no_copy_assign& = delete;
		};

		static_assert(is_detected_v<copy_assignable_t, has_copy_assign>);
		static_assert(!is_detected_v<copy_assignable_t, no_copy_assign>);
		static_assert(is_detected_exact_v<has_copy_assign::copy_assign_return_type, copy_assignable_t, has_copy_assign>);

		template<typename T, typename... Args>
		using has_function_foo_t = decltype(std::declval<T&>().foo(std::declval<Args>()...));

		struct has_function_foo
		{
			auto foo(int, double, float) const noexcept -> void { (void)this; }
		};

		struct no_function_foo { };

		static_assert(!is_detected_v<has_function_foo_t, has_function_foo>);
		static_assert(is_detected_v<has_function_foo_t, has_function_foo, int, double, float>);
		static_assert(!is_detected_v<has_function_foo_t, no_function_foo>);
		static_assert(!is_detected_v<has_function_foo_t, no_function_foo, int, double, float>);

		// test static value
		template<typename T>
		using has_static_value_t = decltype(T::static_value);

		struct has_static_value
		{
			constexpr static int static_value = 42;
		};

		struct no_static_value {};

		static_assert(is_detected_v<has_static_value_t, has_static_value>);
		static_assert(!is_detected_v<has_static_value_t, no_static_value>);

		// test type alias
		template<typename T>
		using value_type_getter = typename T::value_type;

		template<typename T>
		using value_type = detected_or_t<void, value_type_getter, T>;

		struct has_value_type
		{
			using value_type = int;
		};

		struct no_value_type {};

		static_assert(std::is_same_v<value_type<has_value_type>, has_value_type::value_type>);
		static_assert(std::is_same_v<value_type<no_value_type>, void>);
	}
}
