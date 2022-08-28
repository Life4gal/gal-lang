#pragma once

// #include <eve/product_type.hpp>
#include <eve/wide.hpp>

namespace gal::gsl::boost
{
	// namespace vec_detail
	// {
	// 	// ReSharper disable once CppInconsistentNaming
	// 	struct vec4f_impl : eve::struct_support<vec4f_impl, float, float, float, float>
	// 	{
	// 		using eve_disable_ordering = void;
	//
	// 		using value_type = float;
	// 		constexpr static std::size_t size = 4;
	//
	// 		template<typename Tag, eve::like<vec4f_impl> Self>
	// 		constexpr friend auto tagged_dispatch(const Tag&, const eve::as<Self>&) noexcept
	// 		{
	// 			eve::detail::callable_object<Tag> cst;
	// 			return Self{cst(eve::as<value_type>{}), cst(eve::as<value_type>{}), cst(eve::as<value_type>{}), cst(eve::as<value_type>{})};
	// 		}
	//
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) x(this Self&& self) noexcept { return get<0>(std::forward<Self>(self)); }
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) y(this Self&& self) noexcept { return get<1>(std::forward<Self>(self)); }
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) z(this Self&& self) noexcept { return get<2>(std::forward<Self>(self)); }
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) w(this Self&& self) noexcept { return get<3>(std::forward<Self>(self)); }
	//
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) r(this Self&& self) noexcept { return get<0>(std::forward<Self>(self)); }
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) g(this Self&& self) noexcept { return get<1>(std::forward<Self>(self)); }
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) b(this Self&& self) noexcept { return get<2>(std::forward<Self>(self)); }
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) a(this Self&& self) noexcept { return get<3>(std::forward<Self>(self)); }
	//
	// 		[[nodiscard]] constexpr decltype(auto) x() noexcept { return get<0>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) x() const noexcept { return get<0>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) y() noexcept { return get<1>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) y() const noexcept { return get<1>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) z() noexcept { return get<2>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) z() const noexcept { return get<2>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) w() noexcept { return get<3>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) w() const noexcept { return get<3>(*this); }
	//
	// 		[[nodiscard]] constexpr decltype(auto) r() noexcept { return get<0>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) r() const noexcept { return get<0>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) g() noexcept { return get<1>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) g() const noexcept { return get<1>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) b() noexcept { return get<2>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) b() const noexcept { return get<2>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) a() noexcept { return get<3>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) a() const noexcept { return get<3>(*this); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) x(eve::like<vec4f_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).x(); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) y(eve::like<vec4f_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).y(); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) z(eve::like<vec4f_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).z(); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) w(eve::like<vec4f_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).w(); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) r(eve::like<vec4f_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).r(); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) g(eve::like<vec4f_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).g(); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) b(eve::like<vec4f_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).b(); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) a(eve::like<vec4f_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).a(); }
	// 	};
	//
	// 	// ReSharper disable once CppInconsistentNaming
	// 	struct vec4i_impl : eve::struct_support<vec4i_impl, std::int32_t, std::int32_t, std::int32_t, std::int32_t>
	// 	{
	// 		using eve_disable_ordering = void;
	//
	// 		using value_type = std::int32_t;
	// 		constexpr static std::size_t size = 4;
	//
	// 		template<typename Tag, eve::like<vec4i_impl> Self>
	// 		constexpr friend auto tagged_dispatch(const Tag&, const eve::as<Self>&) noexcept
	// 		{
	// 			eve::detail::callable_object<Tag> cst;
	// 			return Self{cst(eve::as<value_type>{}), cst(eve::as<value_type>{}), cst(eve::as<value_type>{}), cst(eve::as<value_type>{})};
	// 		}
	//
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) i1(this Self&& self) noexcept { return get<0>(std::forward<Self>(self)); }
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) i2(this Self&& self) noexcept { return get<1>(std::forward<Self>(self)); }
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) i3(this Self&& self) noexcept { return get<2>(std::forward<Self>(self)); }
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) i4(this Self&& self) noexcept { return get<3>(std::forward<Self>(self)); }
	//
	// 		[[nodiscard]] constexpr decltype(auto) i1() noexcept { return get<0>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) i1() const noexcept { return get<0>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) i2() noexcept { return get<1>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) i2() const noexcept { return get<1>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) i3() noexcept { return get<2>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) i3() const noexcept { return get<2>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) i4() noexcept { return get<3>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) i4() const noexcept { return get<3>(*this); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) i1(eve::like<vec4i_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).i1(); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) i2(eve::like<vec4i_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).i2(); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) i3(eve::like<vec4i_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).i3(); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) i4(eve::like<vec4i_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).i4(); }
	// 	};
	//
	// 	// Matrix 3x4, row major, 3x4 floats
	// 	// ReSharper disable once CppInconsistentNaming
	// 	struct mat34f_impl : eve::struct_support<mat34f_impl, vec4f_impl, vec4f_impl, vec4f_impl>
	// 	{
	// 		using eve_disable_ordering = void;
	//
	// 		using value_type = vec4f_impl;
	// 		constexpr static std::size_t row_size = 3;
	// 		constexpr static std::size_t column_size = 4;
	//
	// 		template<typename Tag, eve::like<mat34f_impl> Self>
	// 		constexpr friend auto tagged_dispatch(const Tag&, const eve::as<Self>&) noexcept
	// 		{
	// 			eve::detail::callable_object<Tag> cst;
	// 			return Self{cst(eve::as<value_type>{}), cst(eve::as<value_type>{}), cst(eve::as<value_type>{}), cst(eve::as<value_type>{})};
	// 		}
	//
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) r1(this Self&& self) noexcept { return get<0>(std::forward<Self>(self)); }
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) r2(this Self&& self) noexcept { return get<1>(std::forward<Self>(self)); }
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) r3(this Self&& self) noexcept { return get<2>(std::forward<Self>(self)); }
	//
	// 		[[nodiscard]] constexpr decltype(auto) r1() noexcept { return get<0>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) r1() const noexcept { return get<0>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) r2() noexcept { return get<1>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) r2() const noexcept { return get<1>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) r3() noexcept { return get<2>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) r3() const noexcept { return get<2>(*this); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) r1(eve::like<mat34f_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).r1(); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) r2(eve::like<mat34f_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).r2(); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) r3(eve::like<mat34f_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).r3(); }
	// 	};
	//
	// 	// Matrix 4x4, row major, 4x4 floats
	// 	// ReSharper disable once CppInconsistentNaming
	// 	struct mat44f_impl : eve::struct_support<mat44f_impl, vec4f_impl, vec4f_impl, vec4f_impl, vec4f_impl>
	// 	{
	// 		using eve_disable_ordering = void;
	//
	// 		using value_type = vec4f_impl;
	// 		constexpr static std::size_t row_size = 4;
	// 		constexpr static std::size_t column_size = 4;
	//
	// 		template<typename Tag, eve::like<mat44f_impl> Self>
	// 		constexpr friend auto tagged_dispatch(const Tag&, const eve::as<Self>&) noexcept
	// 		{
	// 			eve::detail::callable_object<Tag> cst;
	// 			return Self{cst(eve::as<value_type>{}), cst(eve::as<value_type>{}), cst(eve::as<value_type>{}), cst(eve::as<value_type>{})};
	// 		}
	//
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) r1(this Self&& self) noexcept { return get<0>(std::forward<Self>(self)); }
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) r2(this Self&& self) noexcept { return get<1>(std::forward<Self>(self)); }
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) r3(this Self&& self) noexcept { return get<2>(std::forward<Self>(self)); }
	// 		// template<typename Self>
	// 		// [[nodiscard]] constexpr decltype(auto) r4(this Self&& self) noexcept { return get<3>(std::forward<Self>(self)); }
	//
	// 		[[nodiscard]] constexpr decltype(auto) r1() noexcept { return get<0>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) r1() const noexcept { return get<0>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) r2() noexcept { return get<1>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) r2() const noexcept { return get<1>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) r3() noexcept { return get<2>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) r3() const noexcept { return get<2>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) r4() noexcept { return get<3>(*this); }
	// 		[[nodiscard]] constexpr decltype(auto) r4() const noexcept { return get<3>(*this); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) r1(eve::like<mat44f_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).r1(); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) r2(eve::like<mat44f_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).r2(); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) r3(eve::like<mat44f_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).r3(); }
	//
	// 		[[nodiscard]] friend constexpr decltype(auto) r4(eve::like<mat44f_impl> auto&& self) noexcept { return std::forward<decltype(self)>(self).r3(); }
	// 	};
	// }

	// ReSharper disable once CppInconsistentNaming
	using vec4f = eve::wide<float, eve::fixed<4>>;
	// ReSharper disable once CppInconsistentNaming
	using vec4i = eve::wide<std::int32_t, eve::fixed<4>>;
}
