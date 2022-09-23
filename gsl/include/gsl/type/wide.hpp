#pragma once

#include <bit>
#include <eve/wide.hpp>

namespace gal::gsl
{
	namespace type
	{
		using vec4f = eve::wide<float, eve::fixed<4>>;
		using vec4i = eve::wide<std::int32_t, eve::fixed<4>>;
		using vec4u = eve::wide<std::uint32_t, eve::fixed<4>>;

		static_assert(sizeof(vec4f) == sizeof(core::Value));
		static_assert(sizeof(vec4i) == sizeof(core::Value));
		static_assert(sizeof(vec4u) == sizeof(core::Value));
	}

	namespace core
	{
		template<>
		struct ValueCaster<type::vec4f>
		{
			constexpr static auto from(const type::vec4f& data) -> Value { return std::bit_cast<Value>(data); }

			/*constexpr*/
			static auto to(const Value& data) -> type::vec4f { return std::bit_cast<type::vec4f>(data); }
		};

		template<>
		struct ValueCaster<type::vec4i>
		{
			constexpr static auto from(const type::vec4i& data) -> Value { return std::bit_cast<Value>(data); }

			/*constexpr*/
			static auto to(const Value& data) -> type::vec4i { return std::bit_cast<type::vec4i>(data); }
		};

		template<>
		struct ValueCaster<type::vec4u>
		{
			constexpr static auto from(const type::vec4u& data) -> Value { return std::bit_cast<Value>(data); }

			/*constexpr*/
			static auto to(const Value& data) -> type::vec4u { return std::bit_cast<type::vec4u>(data); }
		};
	}
}
