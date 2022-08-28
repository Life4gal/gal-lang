#pragma once

#include <type_traits>
#include <eve/as.hpp>
#include <eve/module/core/constant/core.hpp>
#include <eve/module/core/regular/core.hpp>
#include <eve/module/core/decorator/core.hpp>

namespace gal::gsl::boost::vec
{
	// {arg0, arg1, arg2, arg3}
	template<eve::value T, typename... Args>
		requires std::is_constructible_v<T, Args...>
	[[nodiscard]] constexpr T make(Args&&... args) noexcept { return T{std::forward<Args>(args)...}; }

	// {0, 0, 0, 0}
	template<eve::value T>
	[[nodiscard]] constexpr T make_zero() noexcept { return eve::as_value(eve::zero, eve::as<T>()); }

	// {1, 1, 1, 1}
	template<eve::value T>
	[[nodiscard]] constexpr T make_one() noexcept { return eve::as_value(eve::one, eve::as<T>()); }

	// {T::value_type::max, T::value_type::max, T::value_type::max, T::value_type::max}
	template<eve::value T>
	[[nodiscard]] constexpr T make_max() noexcept { return eve::as_value(eve::valmax, eve::as<T>()); }

	// {T::value_type::min, T::value_type::min, T::value_type::min, T::value_type::min}
	template<eve::value T>
	[[nodiscard]] constexpr T make_min() noexcept { return eve::as_value(eve::valmin, eve::as<T>()); }

	// {0x80000000, 0x80000000, 0x80000000, 0x80000000}
	template<eve::value T>
	[[nodiscard]] constexpr T make_sign_mask() noexcept { return eve::as_value(eve::signmask, eve::as<T>()); }

	// {value, value, value, value)
	template<eve::value T>
	[[nodiscard]] constexpr T make_splat(const typename T::value_type value) noexcept { return eve::as_value(value, eve::as<T>()); }

	// {data[0], data[1], data[2], data[3]}
	template<eve::value T>
	[[nodiscard]] constexpr T make_load(const typename T::value_type* data) noexcept { return T{data}; }

	// generator(std::ptrdiff_t index, std::ptrdiff_t cardinal)
	// {generator(0, cardinal), generator(1, cardinal), generator(2, cardinal), generator(3, cardinal)}
	template<eve::value T, typename Generator>
		requires std::is_constructible_v<T, Generator>
	[[nodiscard]] constexpr T make_load(Generator&& generator) noexcept { return T{std::forward<Generator>(generator)}; }

	// lhs == rhs
	template<eve::value T>
	[[nodiscard]] constexpr auto equal(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::is_equal(lhs, rhs); }

	// lhs == rhs
	// considers that Nan values `are equal`.
	template<eve::value T>
	[[nodiscard]] constexpr auto equal_numeric(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::numeric(eve::is_equal)(lhs, rhs); }

	// lhs == rhs
	// lhs and rhs must be floating point values,
	// eval to true if and only if `lhs` is almost equal to `rhs`.
	// This means that the relative error of confusing is `lhs` and `rhs` is less than `3*eve::eps(eve::as<float>())`
	template<eve::value T>
	[[nodiscard]] constexpr auto equal_almost(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::almost(eve::is_equal)(lhs, rhs); }

	// lhs != rhs
	template<eve::value T>
	[[nodiscard]] constexpr auto not_equal(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::is_not_equal(lhs, rhs); }

	// lhs != rhs
	// considers that Nan values `are not equal`.
	template<eve::value T>
	[[nodiscard]] constexpr auto not_equal_numeric(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::numeric(eve::is_not_equal)(lhs, rhs); }

	// lhs != rhs
	// lhs and rhs must be floating point values,
	// eval to true if and only if `lhs` is almost equal to `rhs`.
	// This means that the relative error of not confusing is `lhs` and `rhs` is greater than `3*eve::eps(eve::as<float>())`
	template<eve::value T>
	[[nodiscard]] constexpr auto not_equal_almost(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::almost(eve::is_not_equal)(lhs, rhs); }

	// lhs > rhs
	template<eve::value T>
	[[nodiscard]] constexpr auto greater(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::is_greater(lhs, rhs); }

	// lhs > rhs
	// lhs and rhs must be floating point values,
	// eval to true if and only if `lhs` is definitely greater than `rhs`
	// This means that `lhs` > `rhs` +`3*eve::eps(eve::as<float>())`
	template<eve::value T>
	[[nodiscard]] constexpr auto greater_definitely(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::definitely(eve::is_greater)(lhs, rhs); }

	// lhs >= rhs
	template<eve::value T>
	[[nodiscard]] constexpr auto greater_equal(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::is_greater_equal(lhs, rhs); }

	// lhs >= rhs
	// see equal_almost
	template<eve::value T>
	[[nodiscard]] constexpr auto greater_equal_almost(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::almost(eve::is_greater_equal)(lhs, rhs); }

	// lhs < rhs
	template<eve::value T>
	[[nodiscard]] constexpr auto less(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::is_less(lhs, rhs); }

	// lhs < rhs
	// lhs and rhs must be floating point values,
	// eval to true if and only if `lhs` is definitely less than `rhs`
	// This means that `lhs` < `rhs` +`3*eve::eps(eve::as<float>())`
	template<eve::value T>
	[[nodiscard]] constexpr auto less_definitely(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::definitely(eve::is_less)(lhs, rhs); }

	// lhs <= rhs
	template<eve::value T>
	[[nodiscard]] constexpr auto less_equal(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::is_less_equal(lhs, rhs); }

	// lhs <= rhs
	// see equal_almost
	template<eve::value T>
	[[nodiscard]] constexpr auto less_equal_almost(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::almost(eve::is_less_equal)(lhs, rhs); }

	// lhs + rhs...
	template<eve::value T, typename... Rhs>
	[[nodiscard]] constexpr auto add(const T& lhs, const Rhs&... rhs) noexcept { return eve::add(lhs, rhs...); }

	// lhs + rhs...
	// Take care that for signed integral entries this kind of addition is not associative at all.
	// This call perform saturated additions in reverse incoming order.
	// {clamped(result[0]), clamped(result[1]), clamped(result[2]), clamped(result[3])}
	template<eve::value T, typename... Rhs>
	[[nodiscard]] constexpr auto add_saturated(const T& lhs, const Rhs&... rhs) noexcept { return eve::saturated(eve::add)(lhs, rhs...); }

	// lhs - rhs...
	template<eve::value T, typename... Rhs>
	[[nodiscard]] constexpr auto sub(const T& lhs, const Rhs&... rhs) noexcept { return eve::sub(lhs, rhs...); }

	// lhs - rhs...
	// Take care that for signed integral entries this kind of addition is not associative at all.
	// This call perform saturated additions in reverse incoming order.
	// {clamped(result[0]), clamped(result[1]), clamped(result[2]), clamped(result[3])}
	template<eve::value T, typename... Rhs>
	[[nodiscard]] constexpr auto sub_saturated(const T& lhs, const Rhs&... rhs) noexcept { return eve::saturated(eve::sub)(lhs, rhs...); }

	// lhs * rhs...
	template<eve::value T, typename... Rhs>
	[[nodiscard]] constexpr auto mul(const T& lhs, const Rhs&... rhs) noexcept { return eve::mul(lhs, rhs...); }

	// lhs * rhs...
	// Take care that for signed integral entries this kind of addition is not associative at all.
	// This call perform saturated additions in reverse incoming order.
	// {clamped(result[0]), clamped(result[1]), clamped(result[2]), clamped(result[3])}
	template<eve::value T, typename... Rhs>
	[[nodiscard]] constexpr auto mul_saturated(const T& lhs, const Rhs&... rhs) noexcept { return eve::saturated(eve::mul)(lhs, rhs...); }

	// lhs / rhs...
	template<eve::value T, typename... Rhs>
	[[nodiscard]] constexpr auto div(const T& lhs, const Rhs&... rhs) noexcept { return eve::div(lhs, rhs...); }

	// lhs / rhs...
	// Take care that for signed integral entries this kind of addition is not associative at all.
	// This call perform saturated additions in reverse incoming order.
	// {clamped(result[0]), clamped(result[1]), clamped(result[2]), clamped(result[3])}
	template<eve::value T, typename... Rhs>
	[[nodiscard]] constexpr auto div_saturated(const T& lhs, const Rhs&... rhs) noexcept { return eve::saturated(eve::div)(lhs, rhs...); }

	// lhs % rhs
	template<eve::value T>
	[[nodiscard]] constexpr auto mod(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::rem(lhs, rhs); }

	// lhs % rhs
	// If `lhs` is ±0 and `rhs` is not zero, ±0 is returned.
	// If `lhs` is ±∞, and `rhs` is not NaN, NaN is returned.
	// If `rhs` is ±0, NaN is returned.
	// If `rhs` is ±∞ and `lhs` is finite, `lhs` is returned.
	// If either argument is a Nan, NaN is returned.
	template<eve::value T>
	[[nodiscard]] constexpr auto mod_toward_zero(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::toward_zero(eve::rem)(lhs, rhs); }

	// lhs % rhs
	template<eve::value T>
	[[nodiscard]] constexpr auto mod_downward(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::downward(eve::rem)(lhs, rhs); }

	// lhs % rhs
	// It is not defined for unsigned values as the result can be negative.
	template<eve::value T>
	[[nodiscard]] constexpr auto mod_upward(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::upward(eve::rem)(lhs, rhs); }

	// lhs % rhs
	// It is not defined for unsigned values as the result can be negative.
	// If `lhs` is ±∞, NaN is returned.
	// If `rhs` is ±0, NaN is returned.
	// If either argument is a Nan, NaN is returned.
	template<eve::value T>
	[[nodiscard]] constexpr auto mod_to_nearest(const T& lhs, const std::type_identity_t<T>& rhs) noexcept { return eve::to_nearest(eve::rem)(lhs, rhs); }
}
