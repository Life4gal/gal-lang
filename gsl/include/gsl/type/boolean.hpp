#pragma once

#include <gsl/type/type_descriptor.hpp>

namespace gal::gsl
{
	namespace type
	{
		struct boolean
		{
		public:
			using data_type = std::int32_t;

		private:
			data_type data;

		public:
			constexpr explicit(false) boolean(const data_type data) noexcept
				: data{data} {}

			constexpr explicit(false) operator data_type() const noexcept { return data; }
		};

		struct boolean_descriptor : public type_descriptor
		{
			friend type_descriptor;
		private:
			[[nodiscard]] constexpr static auto type_size_impl() noexcept -> size_type { return sizeof(boolean::data_type); }

			constexpr static auto zero_out_impl(void* dest, const size_type count) noexcept -> void
			{
				std::ranges::for_each(
						static_cast<boolean*>(dest),
						static_cast<boolean*>(dest) + count,
						[](boolean& d) { d = {0}; });
			}

			constexpr static auto copy_into_impl(void* dest, const void* source, const size_type count) noexcept -> void { std::ranges::copy(static_cast<const boolean*>(source), static_cast<const boolean*>(source) + count, static_cast<boolean*>(dest)); }

			constexpr static auto move_into_impl(void* dest, void* source, const size_type count) noexcept -> void { std::ranges::move(static_cast<boolean*>(source), static_cast<boolean*>(source) + count, static_cast<boolean*>(dest)); }
		};
	}// namespace type

	namespace core
	{
		template<>
		class ValueCaster<type::boolean> : public value_caster_specified
		{
		public:
			constexpr static auto from(const type::boolean data) noexcept -> Value { return {.signed_integer_32 = {data, 0, 0, 0}}; }

			constexpr static auto to(const Value& data) noexcept -> type::boolean { return data.signed_integer_32[0]; }
		};
	}
}
