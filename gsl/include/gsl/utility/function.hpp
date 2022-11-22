#pragma once

#include <type_traits>
#include <ranges>
#include <functional>

namespace gal::gsl::utility
{
	template<typename FunctionType>
	struct y_combinator
	{
		using function_type = FunctionType;

		function_type function;

		template<typename... Args>
		constexpr auto operator()(Args&&... args) const
		noexcept(std::is_nothrow_invocable_v<function_type, decltype(*this), Args...>) -> decltype(auto)
		{
			// we pass ourselves to f, then the arguments.
			// the lambda should take the first argument as `auto&& recurse` or similar.
			return function(*this, std::forward<Args>(args)...);
		}
	};

	template<typename F>
	y_combinator(F) -> y_combinator<std::decay_t<F>>;

	// The return value of 'Function' is not bool. Discard it.
	template<typename Iterator, typename Function, typename... Iterators>
		requires(!std::is_same_v<
				std::invoke_result_t<
					Function,
					typename std::iterator_traits<Iterator>::reference,
					typename std::iterator_traits<Iterators>::reference>,
				bool> &&
			...)
	constexpr auto zip_invoke(
			Iterator begin,
			Iterator end,
			Function&& function,
			Iterators ... iterators) -> void
	{
		auto real_function = [&function](auto&... is)
		{
			std::invoke(function, (*is)...);
			((++is), ...);
		};

		while (begin != end) { real_function(begin, iterators...); }
	}

	// The return value of 'Function' is bool, determine whether to continue according to its return value
	template<typename Iterator, typename Function, typename... Iterators>
		requires(std::is_same_v<
				std::invoke_result_t<
					Function,
					typename std::iterator_traits<Iterator>::reference,
					typename std::iterator_traits<Iterators>::reference>,
				bool> &&
			...)
	constexpr auto zip_invoke(
			Iterator begin,
			Iterator end,
			Function&& function,
			Iterators ... iterators) -> void
	{
		auto real_function = [&function](auto&... is)
		{
			if (!std::invoke(function, (*is)...)) { return false; }
			((++is), ...);
			return true;
		};

		while (begin != end && real_function(begin, iterators...)) {}
	}

	// range
	template<typename Function, typename... Iterator>
	constexpr auto zip_invoke(
			std::ranges::range auto&& r,
			Function&& function,
			Iterator ... iterator) -> void requires requires
	{
		utility::zip_invoke(
				std::ranges::begin(r),
				std::ranges::end(r),
				std::forward<Function>(function),
				iterator...);
	}
	{
		utility::zip_invoke(
				std::ranges::begin(r),
				std::ranges::end(r),
				std::forward<Function>(function),
				iterator...);
	}
}
