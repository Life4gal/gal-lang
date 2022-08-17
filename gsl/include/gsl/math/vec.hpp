#pragma once

#include <eve/product_type.hpp>

namespace gal::gsl::math
{
	enum class vec_fill_tag
	{
		zero,
		duplicate,
	};

	// using vec4f = eve::wide<float, eve::fixed<4>>;
	// ReSharper disable once CppInconsistentNaming
	struct vec4f : eve::struct_support<vec4f, float, float, float, float>
	{
		using eve_disable_ordering = void;

		using value_type = float;
		constexpr static std::size_t size = 4;

		constexpr vec4f() noexcept = default;

		template<typename Arg>
			requires std::is_convertible_v<Arg, value_type>
		explicit constexpr vec4f(Arg arg, const vec_fill_tag t = vec_fill_tag::zero) noexcept(std::is_nothrow_convertible_v<Arg, value_type>)
			: struct_support{
					static_cast<value_type>(arg),
					t == vec_fill_tag::zero ? value_type{} : static_cast<value_type>(arg),
					t == vec_fill_tag::zero ? value_type{} : static_cast<value_type>(arg),
					t == vec_fill_tag::zero ? value_type{} : static_cast<value_type>(arg)} {}

		template<typename... Args>
			requires(sizeof...(Args) == size) && (std::is_convertible_v<Args, value_type> && ...)
		explicit constexpr vec4f(Args ... args) noexcept((std::is_nothrow_convertible_v<Args, value_type> && ...))
			: struct_support{static_cast<value_type>(args)...} {}

		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) x(this Self&& self) noexcept { return get<0>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) y(this Self&& self) noexcept { return get<1>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) z(this Self&& self) noexcept { return get<2>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) w(this Self&& self) noexcept { return get<3>(std::forward<Self>(self)); }

		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) r(this Self&& self) noexcept { return get<0>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) g(this Self&& self) noexcept { return get<1>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) b(this Self&& self) noexcept { return get<2>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) a(this Self&& self) noexcept { return get<3>(std::forward<Self>(self)); }

		[[nodiscard]] constexpr decltype(auto) x() noexcept { return get<0>(*this); }
		[[nodiscard]] constexpr decltype(auto) x() const noexcept { return get<0>(*this); }
		[[nodiscard]] constexpr decltype(auto) y() noexcept { return get<1>(*this); }
		[[nodiscard]] constexpr decltype(auto) y() const noexcept { return get<1>(*this); }
		[[nodiscard]] constexpr decltype(auto) z() noexcept { return get<2>(*this); }
		[[nodiscard]] constexpr decltype(auto) z() const noexcept { return get<2>(*this); }
		[[nodiscard]] constexpr decltype(auto) w() noexcept { return get<3>(*this); }
		[[nodiscard]] constexpr decltype(auto) w() const noexcept { return get<3>(*this); }

		[[nodiscard]] constexpr decltype(auto) r() noexcept { return get<0>(*this); }
		[[nodiscard]] constexpr decltype(auto) r() const noexcept { return get<0>(*this); }
		[[nodiscard]] constexpr decltype(auto) g() noexcept { return get<1>(*this); }
		[[nodiscard]] constexpr decltype(auto) g() const noexcept { return get<1>(*this); }
		[[nodiscard]] constexpr decltype(auto) b() noexcept { return get<2>(*this); }
		[[nodiscard]] constexpr decltype(auto) b() const noexcept { return get<2>(*this); }
		[[nodiscard]] constexpr decltype(auto) a() noexcept { return get<3>(*this); }
		[[nodiscard]] constexpr decltype(auto) a() const noexcept { return get<3>(*this); }

		[[nodiscard]] explicit constexpr operator struct vec3f() const noexcept;
	};

	// using vec3f = eve::wide<float, eve::fixed<4>>;
	// ReSharper disable once CppInconsistentNaming
	struct vec3f : eve::struct_support<vec3f, float, float, float, float>
	{
		using eve_disable_ordering = void;

		using value_type = float;
		constexpr static std::size_t size = 3;
		constexpr static std::size_t pack_size = 4;

		constexpr vec3f() noexcept = default;

		template<typename Arg>
			requires std::is_convertible_v<Arg, value_type>
		explicit constexpr vec3f(Arg arg, const vec_fill_tag t = vec_fill_tag::zero) noexcept(std::is_nothrow_convertible_v<Arg, value_type>)
			: struct_support{
					static_cast<value_type>(arg),
					t == vec_fill_tag::zero ? value_type{} : static_cast<value_type>(arg),
					t == vec_fill_tag::zero ? value_type{} : static_cast<value_type>(arg),
					t == vec_fill_tag::zero ? value_type{} : static_cast<value_type>(arg)} {}

		template<typename... Args>
			requires(sizeof...(Args) == size) && (std::is_convertible_v<Args, value_type> && ...)
		explicit constexpr vec3f(Args ... args) noexcept((std::is_nothrow_convertible_v<Args, value_type> && ...))
			: struct_support{static_cast<value_type>(args)..., 0} {}

		template<typename... Args>
			requires(sizeof...(Args) == pack_size) && (std::is_convertible_v<Args, value_type> && ...)
		explicit constexpr vec3f(Args ... args) noexcept((std::is_nothrow_convertible_v<Args, value_type> && ...))
			: struct_support{static_cast<value_type>(args)...} {}

		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) x(this Self&& self) noexcept { return get<0>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) y(this Self&& self) noexcept { return get<1>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) z(this Self&& self) noexcept { return get<2>(std::forward<Self>(self)); }

		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) r(this Self&& self) noexcept { return get<0>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) g(this Self&& self) noexcept { return get<1>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) b(this Self&& self) noexcept { return get<2>(std::forward<Self>(self)); }

		[[nodiscard]] constexpr decltype(auto) x() noexcept { return get<0>(*this); }
		[[nodiscard]] constexpr decltype(auto) x() const noexcept { return get<0>(*this); }
		[[nodiscard]] constexpr decltype(auto) y() noexcept { return get<1>(*this); }
		[[nodiscard]] constexpr decltype(auto) y() const noexcept { return get<1>(*this); }
		[[nodiscard]] constexpr decltype(auto) z() noexcept { return get<2>(*this); }
		[[nodiscard]] constexpr decltype(auto) z() const noexcept { return get<2>(*this); }

		[[nodiscard]] constexpr decltype(auto) r() noexcept { return get<0>(*this); }
		[[nodiscard]] constexpr decltype(auto) r() const noexcept { return get<0>(*this); }
		[[nodiscard]] constexpr decltype(auto) g() noexcept { return get<1>(*this); }
		[[nodiscard]] constexpr decltype(auto) g() const noexcept { return get<1>(*this); }
		[[nodiscard]] constexpr decltype(auto) b() noexcept { return get<2>(*this); }
		[[nodiscard]] constexpr decltype(auto) b() const noexcept { return get<2>(*this); }

		[[nodiscard]] explicit constexpr operator vec4f() const noexcept { return vec4f{x(), y(), z(), 0}; }
	};

	constexpr vec4f::operator vec3f() const noexcept { return vec3f{x(), y(), z()}; }

	// using vec4i = eve::wide<std::int32_t, eve::fixed<4>>;
	// ReSharper disable once CppInconsistentNaming
	struct vec4i : eve::struct_support<vec4i, std::int32_t, std::int32_t, std::int32_t, std::int32_t>
	{
		using eve_disable_ordering = void;

		using value_type = std::int32_t;
		constexpr static std::size_t size = 4;

		constexpr vec4i() noexcept = default;

		template<typename Arg>
			requires std::is_convertible_v<Arg, value_type>
		explicit constexpr vec4i(Arg arg, const vec_fill_tag t = vec_fill_tag::zero) noexcept(std::is_nothrow_convertible_v<Arg, value_type>)
			: struct_support{
					static_cast<value_type>(arg),
					t == vec_fill_tag::zero ? value_type{} : static_cast<value_type>(arg),
					t == vec_fill_tag::zero ? value_type{} : static_cast<value_type>(arg),
					t == vec_fill_tag::zero ? value_type{} : static_cast<value_type>(arg)} {}

		template<typename... Args>
			requires(sizeof...(Args) == size) && (std::is_convertible_v<Args, value_type> && ...)
		explicit constexpr vec4i(Args ... args) noexcept((std::is_nothrow_convertible_v<Args, value_type> && ...))
			: struct_support{static_cast<value_type>(args)...} {}

		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) i1(this Self&& self) noexcept { return get<0>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) i2(this Self&& self) noexcept { return get<1>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) i3(this Self&& self) noexcept { return get<2>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) i4(this Self&& self) noexcept { return get<3>(std::forward<Self>(self)); }

		[[nodiscard]] constexpr decltype(auto) i1() noexcept { return get<0>(*this); }
		[[nodiscard]] constexpr decltype(auto) i1() const noexcept { return get<0>(*this); }
		[[nodiscard]] constexpr decltype(auto) i2() noexcept { return get<1>(*this); }
		[[nodiscard]] constexpr decltype(auto) i2() const noexcept { return get<1>(*this); }
		[[nodiscard]] constexpr decltype(auto) i3() noexcept { return get<2>(*this); }
		[[nodiscard]] constexpr decltype(auto) i3() const noexcept { return get<2>(*this); }
		[[nodiscard]] constexpr decltype(auto) i4() noexcept { return get<3>(*this); }
		[[nodiscard]] constexpr decltype(auto) i4() const noexcept { return get<3>(*this); }
	};

	// Matrix 3x3, column major, 3x4 floats
	// ReSharper disable once CppInconsistentNaming
	struct mat33f : eve::struct_support<mat33f, vec3f, vec3f, vec3f>
	{
		using eve_disable_ordering = void;

		using value_type = vec3f;
		constexpr static std::size_t size = 3;

		template<typename Arg>
			requires std::is_constructible_v<Arg, value_type>
		explicit constexpr mat33f(Arg arg, const vec_fill_tag t = vec_fill_tag::zero) noexcept(std::is_nothrow_constructible_v<Arg, value_type>)
			: struct_support{
					value_type{arg},
					t == vec_fill_tag::zero ? value_type{} : value_type{arg},
					t == vec_fill_tag::zero ? value_type{} : value_type{arg}} {}

		template<typename... Args>
			requires(sizeof...(Args) == size) && (std::is_constructible_v<Args, value_type> && ...)
		explicit constexpr mat33f(Args ... args) noexcept((std::is_nothrow_constructible_v<Args, value_type> && ...))
			: struct_support{value_type{args}...} {}

		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) r1(this Self&& self) noexcept { return get<0>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) r2(this Self&& self) noexcept { return get<1>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) r3(this Self&& self) noexcept { return get<2>(std::forward<Self>(self)); }

		[[nodiscard]] constexpr decltype(auto) r1() noexcept { return get<0>(*this); }
		[[nodiscard]] constexpr decltype(auto) r1() const noexcept { return get<0>(*this); }
		[[nodiscard]] constexpr decltype(auto) r2() noexcept { return get<1>(*this); }
		[[nodiscard]] constexpr decltype(auto) r2() const noexcept { return get<1>(*this); }
		[[nodiscard]] constexpr decltype(auto) r3() noexcept { return get<2>(*this); }
		[[nodiscard]] constexpr decltype(auto) r3() const noexcept { return get<2>(*this); }

		explicit constexpr operator struct mat44f() const noexcept;
	};

	// Matrix 4x4, column major, 4x4 floats
	// ReSharper disable once CppInconsistentNaming
	struct mat44f : eve::struct_support<mat44f, vec4f, vec4f, vec4f, vec4f>
	{
		using eve_disable_ordering = void;

		using value_type = vec4f;
		constexpr static std::size_t size = 4;

		template<typename Arg>
			requires std::is_constructible_v<Arg, value_type>
		explicit constexpr mat44f(Arg arg, const vec_fill_tag t = vec_fill_tag::zero) noexcept(std::is_nothrow_constructible_v<Arg, value_type>)
			: struct_support{
					value_type{arg},
					t == vec_fill_tag::zero ? value_type{} : value_type{arg},
					t == vec_fill_tag::zero ? value_type{} : value_type{arg},
					t == vec_fill_tag::zero ? value_type{} : value_type{arg}} {}

		template<typename... Args>
			requires(sizeof...(Args) == size) && (std::is_constructible_v<Args, value_type> && ...)
		explicit constexpr mat44f(Args ... args) noexcept((std::is_nothrow_constructible_v<Args, value_type> && ...))
			: struct_support{value_type{args}...} {}

		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) r1(this Self&& self) noexcept { return get<0>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) r2(this Self&& self) noexcept { return get<1>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) r3(this Self&& self) noexcept { return get<2>(std::forward<Self>(self)); }
		// template<typename Self>
		// [[nodiscard]] constexpr decltype(auto) r4(this Self&& self) noexcept { return get<3>(std::forward<Self>(self)); }

		[[nodiscard]] constexpr decltype(auto) r1() noexcept { return get<0>(*this); }
		[[nodiscard]] constexpr decltype(auto) r1() const noexcept { return get<0>(*this); }
		[[nodiscard]] constexpr decltype(auto) r2() noexcept { return get<1>(*this); }
		[[nodiscard]] constexpr decltype(auto) r2() const noexcept { return get<1>(*this); }
		[[nodiscard]] constexpr decltype(auto) r3() noexcept { return get<2>(*this); }
		[[nodiscard]] constexpr decltype(auto) r3() const noexcept { return get<2>(*this); }
		[[nodiscard]] constexpr decltype(auto) r4() noexcept { return get<3>(*this); }
		[[nodiscard]] constexpr decltype(auto) r4() const noexcept { return get<3>(*this); }

		explicit constexpr operator struct mat33f() const noexcept { return mat33f{r1(), r2(), r3()}; }
	};

	constexpr mat33f::operator mat44f() const noexcept { return mat44f{r1(), r2(), r3(), mat44f::value_type{}}; }
}
