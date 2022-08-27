#pragma once

#include <variant>

namespace gal::gsl::boost
{
	template<typename... Ts>
	using variant = ::std::variant<Ts...>;

	template<typename Visitor, typename... Variants>
	constexpr decltype(auto) visit(Visitor&& visitor, Variants&&... variants) { return ::std::visit(::std::forward<Visitor>(visitor), ::std::forward<Variants>(variants)...); }

	template<typename Return, typename Visitor, typename... Variants>
	constexpr decltype(auto) visit(Visitor&& visitor, Variants&&... variants) { return ::std::visit<Return>(::std::forward<Visitor>(visitor), ::std::forward<Variants>(variants)...); }

	template<typename T, typename... Ts>
	[[nodiscard]] constexpr bool holds_alternative(const variant<Ts...>& variant) noexcept { return ::std::holds_alternative<T>(variant); }

	template<std::size_t Index, typename... Ts>
	[[nodiscard]] constexpr decltype(auto) get(variant<Ts...>& variant) { return ::std::get<Index>(variant); }

	template<std::size_t Index, typename... Ts>
	[[nodiscard]] constexpr decltype(auto) get(const variant<Ts...>& variant) { return ::std::get<Index>(variant); }

	template<std::size_t Index, typename... Ts>
	[[nodiscard]] constexpr decltype(auto) get(variant<Ts...>&& variant) { return ::std::get<Index>(::std::forward<decltype(variant)>(variant)); }

	template<std::size_t Index, typename... Ts>
	[[nodiscard]] constexpr decltype(auto) get(const variant<Ts...>&& variant) { return ::std::get<Index>(::std::forward<decltype(variant)>(variant)); }

	template<typename T, typename... Ts>
	[[nodiscard]] constexpr decltype(auto) get(variant<Ts...>& variant) { return ::std::get<T>(variant); }

	template<typename T, typename... Ts>
	[[nodiscard]] constexpr decltype(auto) get(const variant<Ts...>& variant) { return ::std::get<T>(variant); }

	template<typename T, typename... Ts>
	[[nodiscard]] constexpr decltype(auto) get(variant<Ts...>&& variant) { return ::std::get<T>(::std::forward<decltype(variant)>(variant)); }

	template<typename T, typename... Ts>
	[[nodiscard]] constexpr decltype(auto) get(const variant<Ts...>&& variant) { return ::std::get<T>(::std::forward<decltype(variant)>(variant)); }

	template<std::size_t Index, typename... Ts>
	[[nodiscard]] constexpr decltype(auto) get_if(variant<Ts...>* variant) noexcept { return ::std::get_if<Index>(variant); }

	template<std::size_t Index, typename... Ts>
	[[nodiscard]] constexpr decltype(auto) get_if(const variant<Ts...>* variant) noexcept { return ::std::get_if<Index>(variant); }

	template<typename T, typename... Ts>
	[[nodiscard]] constexpr decltype(auto) get_if(variant<Ts...>* variant) noexcept { return ::std::get_if<T>(variant); }

	template<typename T, typename... Ts>
	[[nodiscard]] constexpr decltype(auto) get_if(const variant<Ts...>* variant) noexcept { return ::std::get_if<T>(variant); }
}
