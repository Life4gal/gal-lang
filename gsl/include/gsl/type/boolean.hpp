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
		};
	}// namespace type

	namespace core
	{
		template<>
		struct ValueCaster<type::boolean>
		{
			constexpr static auto from(const type::boolean data) noexcept -> Value { return {.signed_integer_32 = {data, 0, 0, 0}}; }

			constexpr static auto to(const Value& data) noexcept -> type::boolean { return data.signed_integer_32[0]; }
		};
	}
}
