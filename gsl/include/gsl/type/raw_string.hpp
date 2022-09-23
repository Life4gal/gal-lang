#pragma once

#include <gsl/type/type_descriptor.hpp>

namespace gal::gsl
{
	namespace type
	{
		struct raw_string
		{
		public:
			// must be string::const_pointer!
			using data_type = const char*;

		private:
			data_type data;

		public:
			constexpr explicit(false) raw_string(const data_type data) noexcept
				: data{data} {}

			constexpr explicit(false) operator data_type() const noexcept { return data; }
		};

		struct raw_string_descriptor : public type_descriptor
		{
			friend type_descriptor;

		private:
			[[nodiscard]] constexpr static auto type_size_impl() noexcept -> size_type { return sizeof(raw_string::data_type); }

			constexpr static auto copy_into_impl(void* dest, const void* source, const size_type count) -> void { std::ranges::copy(static_cast<const raw_string*>(source), static_cast<const raw_string*>(source) + count, static_cast<raw_string*>(dest)); }

			constexpr static auto move_into_impl(void* dest, void* source, const size_type count) -> void { std::ranges::move(static_cast<raw_string*>(source), static_cast<raw_string*>(source) + count, static_cast<raw_string*>(dest)); }
		};
	}

	namespace core
	{
		template<>
		struct ValueCaster<type::raw_string>
		{
			constexpr static auto from(const type::raw_string data) noexcept -> Value { return {.raw_observer = data}; }

			constexpr static auto to(const Value& data) noexcept -> type::raw_string { return static_cast<const char*>(data.raw_observer); }
		};
	}
}
