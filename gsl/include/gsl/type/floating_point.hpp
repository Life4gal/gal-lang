#pragma once

#include <gsl/type/type_descriptor.hpp>

namespace gal::gsl
{
	namespace type
	{
		struct single_precision
		{
		public:
			using data_type = float;

		private:
			data_type data;

		public:
			constexpr explicit(false) single_precision(const data_type data) noexcept
				: data{data} {}

			constexpr explicit(false) operator data_type() const noexcept { return data; }
		};

		struct double_precision
		{
		public:
			using data_type = double;

		private:
			data_type data;

		public:
			constexpr explicit(false) double_precision(const data_type data) noexcept
				: data{data} {}

			constexpr explicit(false) operator data_type() const noexcept { return data; }
		};

		struct single_precision_descriptor : public type_descriptor
		{
			friend type_descriptor;

		private:
			[[nodiscard]] constexpr static auto type_size_impl() noexcept -> size_type { return sizeof(single_precision::data_type); }
		};

		struct double_precision_descriptor : public type_descriptor
		{
			friend type_descriptor;

		private:
			[[nodiscard]] constexpr static auto type_size_impl() noexcept -> size_type { return sizeof(double_precision::data_type); }
		};
	}

	namespace core
	{
		template<>
		struct ValueCaster<type::single_precision>
		{
			constexpr static auto from(const type::single_precision data) noexcept -> Value { return {.single_precision = {data, 0, 0, 0}}; }

			constexpr static auto to(const Value& data) noexcept -> type::single_precision { return data.single_precision[0]; }
		};

		template<>
		struct ValueCaster<type::double_precision>
		{
			constexpr static auto from(const type::double_precision data) noexcept -> Value { return {.double_precision = {data, 0}}; }

			constexpr static auto to(const Value& data) noexcept -> type::double_precision { return data.double_precision[0]; }
		};
	}
}
