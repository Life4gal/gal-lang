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

			constexpr static auto zero_out_impl(void* dest, const size_type count) noexcept -> void
			{
				std::ranges::for_each(
						static_cast<single_precision*>(dest),
						static_cast<single_precision*>(dest) + count,
						[](single_precision& d) { d = {0}; });
			}

			constexpr static auto copy_into_impl(void* dest, const void* source, const size_type count) -> void { std::ranges::copy(static_cast<const single_precision*>(source), static_cast<const single_precision*>(source) + count, static_cast<single_precision*>(dest)); }

			constexpr static auto move_into_impl(void* dest, void* source, const size_type count) -> void { std::ranges::move(static_cast<single_precision*>(source), static_cast<single_precision*>(source) + count, static_cast<single_precision*>(dest)); }
		};

		struct double_precision_descriptor : public type_descriptor
		{
			friend type_descriptor;

		private:
			[[nodiscard]] constexpr static auto type_size_impl() noexcept -> size_type { return sizeof(double_precision::data_type); }

			constexpr static auto zero_out_impl(void* dest, const size_type count) noexcept -> void
			{
				std::ranges::for_each(
						static_cast<double_precision*>(dest),
						static_cast<double_precision*>(dest) + count,
						[](double_precision& d) { d = {0}; });
			}

			constexpr static auto copy_into_impl(void* dest, const void* source, const size_type count) -> void { std::ranges::copy(static_cast<const double_precision*>(source), static_cast<const double_precision*>(source) + count, static_cast<double_precision*>(dest)); }

			constexpr static auto move_into_impl(void* dest, void* source, const size_type count) -> void { std::ranges::move(static_cast<double_precision*>(source), static_cast<double_precision*>(source) + count, static_cast<double_precision*>(dest)); }
		};
	}

	namespace core
	{
		template<>
		class ValueCaster<type::single_precision> : public value_caster_specified
		{
		public:
			constexpr static auto from(const type::single_precision data) noexcept -> Value { return {.single_precision = {data, 0, 0, 0}}; }

			constexpr static auto to(const Value& data) noexcept -> type::single_precision { return data.single_precision[0]; }
		};

		template<>
		class ValueCaster<type::double_precision> : public value_caster_specified
		{
		public:
			constexpr static auto from(const type::double_precision data) noexcept -> Value { return {.double_precision = {data, 0}}; }

			constexpr static auto to(const Value& data) noexcept -> type::double_precision { return data.double_precision[0]; }
		};
	}
}
