#pragma once

#include <bit>
#include <eve/wide.hpp>
#include <gsl/type/type_descriptor.hpp>

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

		struct vec4f_descriptor : public type_descriptor
		{
			friend type_descriptor;

		private:
			[[nodiscard]] constexpr static auto type_size_impl() noexcept -> size_type { return sizeof(vec4f); }

			constexpr static auto zero_out_impl(void* dest, const size_type count) noexcept -> void
			{
				std::ranges::for_each(
						static_cast<vec4f*>(dest),
						static_cast<vec4f*>(dest) + count,
						[](vec4f& d) { d = {0}; });
			}

			constexpr static auto copy_into_impl(void* dest, const void* source, const size_type count) -> void { std::ranges::copy(static_cast<const vec4f*>(source), static_cast<const vec4f*>(source) + count, static_cast<vec4f*>(dest)); }

			constexpr static auto move_into_impl(void* dest, void* source, const size_type count) -> void { std::ranges::move(static_cast<vec4f*>(source), static_cast<vec4f*>(source) + count, static_cast<vec4f*>(dest)); }
		};

		struct vec4i_descriptor : public type_descriptor
		{
			friend type_descriptor;

		private:
			[[nodiscard]] constexpr static auto type_size_impl() noexcept -> size_type { return sizeof(vec4i); }

			constexpr static auto zero_out_impl(void* dest, const size_type count) noexcept -> void
			{
				std::ranges::for_each(
						static_cast<vec4i*>(dest),
						static_cast<vec4i*>(dest) + count,
						[](vec4i& d) { d = {0}; });
			}

			constexpr static auto copy_into_impl(void* dest, const void* source, const size_type count) -> void { std::ranges::copy(static_cast<const vec4i*>(source), static_cast<const vec4i*>(source) + count, static_cast<vec4i*>(dest)); }

			constexpr static auto move_into_impl(void* dest, void* source, const size_type count) -> void { std::ranges::move(static_cast<vec4i*>(source), static_cast<vec4i*>(source) + count, static_cast<vec4i*>(dest)); }
		};

		struct vec4u_descriptor : public type_descriptor
		{
			friend type_descriptor;

		private:
			[[nodiscard]] constexpr static auto type_size_impl() noexcept -> size_type { return sizeof(vec4u); }

			constexpr static auto zero_out_impl(void* dest, const size_type count) noexcept -> void
			{
				std::ranges::for_each(
						static_cast<vec4u*>(dest),
						static_cast<vec4u*>(dest) + count,
						[](vec4u& d) { d = {0}; });
			}

			constexpr static auto copy_into_impl(void* dest, const void* source, const size_type count) -> void { std::ranges::copy(static_cast<const vec4u*>(source), static_cast<const vec4u*>(source) + count, static_cast<vec4u*>(dest)); }

			constexpr static auto move_into_impl(void* dest, void* source, const size_type count) -> void { std::ranges::move(static_cast<vec4u*>(source), static_cast<vec4u*>(source) + count, static_cast<vec4u*>(dest)); }
		};
	}

	namespace core
	{
		template<>
		class ValueCaster<type::vec4f> : public value_caster_specified
		{
		public:
			constexpr static auto from(const type::vec4f& data) -> Value { return std::bit_cast<Value>(data); }

			/*constexpr*/
			static auto to(const Value& data) -> type::vec4f { return std::bit_cast<type::vec4f>(data); }
		};

		template<>
		class ValueCaster<type::vec4i> : public value_caster_specified
		{
		public:
			constexpr static auto from(const type::vec4i& data) -> Value { return std::bit_cast<Value>(data); }

			/*constexpr*/
			static auto to(const Value& data) -> type::vec4i { return std::bit_cast<type::vec4i>(data); }
		};

		template<>
		class ValueCaster<type::vec4u> : public value_caster_specified
		{
		public:
			constexpr static auto from(const type::vec4u& data) -> Value { return std::bit_cast<Value>(data); }

			/*constexpr*/
			static auto to(const Value& data) -> type::vec4u { return std::bit_cast<type::vec4u>(data); }
		};
	}
}
