#pragma once

#include <cstdint>
#include <gsl/type/type_descriptor.hpp>

namespace gal::gsl
{
	namespace type
	{
		struct signed_integer
		{
		public:
			using data_type = std::int32_t;

		private:
			data_type data;

		public:
			constexpr explicit(false) signed_integer(const data_type data) noexcept
				: data{data} {}

			constexpr explicit(false) operator data_type() const noexcept { return data; }
		};

		struct unsigned_integer
		{
		public:
			using data_type = std::uint32_t;

		private:
			data_type data;

		public:
			constexpr explicit(false) unsigned_integer(const data_type data) noexcept
				: data{data} {}

			constexpr explicit(false) operator data_type() const noexcept { return data; }
		};

		struct signed_long_integer
		{
		public:
			using data_type = std::int64_t;

		private:
			data_type data;

		public:
			constexpr explicit(false) signed_long_integer(const data_type data) noexcept
				: data{data} {}

			constexpr explicit(false) operator data_type() const noexcept { return data; }
		};

		struct unsigned_long_integer
		{
		public:
			using data_type = std::uint64_t;

		private:
			data_type data;

		public:
			constexpr explicit(false) unsigned_long_integer(const data_type data) noexcept
				: data{data} {}

			constexpr explicit(false) operator data_type() const noexcept { return data; }
		};

		struct signed_integer_descriptor : public type_descriptor
		{
			friend type_descriptor;

		private:
			[[nodiscard]] constexpr static auto type_size_impl() noexcept -> size_type { return sizeof(signed_integer::data_type); }

			constexpr static auto zero_out_impl(void* dest, const size_type count) noexcept -> void
			{
				std::ranges::for_each(
						static_cast<signed_integer*>(dest),
						static_cast<signed_integer*>(dest) + count,
						[](signed_integer& d) { d = {0}; });
			}

			constexpr static auto copy_into_impl(void* dest, const void* source, const size_type count) -> void { std::ranges::copy(static_cast<const signed_integer*>(source), static_cast<const signed_integer*>(source) + count, static_cast<signed_integer*>(dest)); }

			constexpr static auto move_into_impl(void* dest, void* source, const size_type count) -> void { std::ranges::move(static_cast<signed_integer*>(source), static_cast<signed_integer*>(source) + count, static_cast<signed_integer*>(dest)); }
		};

		struct unsigned_integer_descriptor : public type_descriptor
		{
			friend type_descriptor;

		private:
			[[nodiscard]] constexpr static auto type_size_impl() noexcept -> size_type { return sizeof(unsigned_integer::data_type); }

			constexpr static auto zero_out_impl(void* dest, const size_type count) noexcept -> void
			{
				std::ranges::for_each(
						static_cast<unsigned_integer*>(dest),
						static_cast<unsigned_integer*>(dest) + count,
						[](unsigned_integer& d) { d = {0}; });
			}

			constexpr static auto copy_into_impl(void* dest, const void* source, const size_type count) -> void { std::ranges::copy(static_cast<const unsigned_integer*>(source), static_cast<const unsigned_integer*>(source) + count, static_cast<unsigned_integer*>(dest)); }

			constexpr static auto move_into_impl(void* dest, void* source, const size_type count) -> void { std::ranges::move(static_cast<unsigned_integer*>(source), static_cast<unsigned_integer*>(source) + count, static_cast<unsigned_integer*>(dest)); }
		};

		struct signed_long_integer_descriptor : public type_descriptor
		{
			friend type_descriptor;

		private:
			[[nodiscard]] constexpr static auto type_size_impl() noexcept -> size_type { return sizeof(signed_long_integer::data_type); }

			constexpr static auto zero_out_impl(void* dest, const size_type count) noexcept -> void
			{
				std::ranges::for_each(
						static_cast<signed_long_integer*>(dest),
						static_cast<signed_long_integer*>(dest) + count,
						[](signed_long_integer& d) { d = {0}; });
			}

			constexpr static auto copy_into_impl(void* dest, const void* source, const size_type count) -> void { std::ranges::copy(static_cast<const signed_long_integer*>(source), static_cast<const signed_long_integer*>(source) + count, static_cast<signed_long_integer*>(dest)); }

			constexpr static auto move_into_impl(void* dest, void* source, const size_type count) -> void { std::ranges::move(static_cast<signed_long_integer*>(source), static_cast<signed_long_integer*>(source) + count, static_cast<signed_long_integer*>(dest)); }
		};

		struct unsigned_long_integer_descriptor : public type_descriptor
		{
			friend type_descriptor;

		private:
			[[nodiscard]] constexpr static auto type_size_impl() noexcept -> size_type { return sizeof(unsigned_long_integer::data_type); }

			constexpr static auto zero_out_impl(void* dest, const size_type count) noexcept -> void
			{
				std::ranges::for_each(
						static_cast<unsigned_long_integer*>(dest),
						static_cast<unsigned_long_integer*>(dest) + count,
						[](unsigned_long_integer& d) { d = {0}; });
			}

			constexpr static auto copy_into_impl(void* dest, const void* source, const size_type count) -> void { std::ranges::copy(static_cast<const unsigned_long_integer*>(source), static_cast<const unsigned_long_integer*>(source) + count, static_cast<unsigned_long_integer*>(dest)); }

			constexpr static auto move_into_impl(void* dest, void* source, const size_type count) -> void { std::ranges::move(static_cast<unsigned_long_integer*>(source), static_cast<unsigned_long_integer*>(source) + count, static_cast<unsigned_long_integer*>(dest)); }
		};
	}

	namespace core
	{
		template<>
		struct ValueCaster<type::signed_integer>
		{
			constexpr static auto from(const type::signed_integer data) noexcept -> Value { return {.signed_integer_32 = {data, 0, 0, 0}}; }

			constexpr static auto to(const Value& data) noexcept -> type::signed_integer { return data.signed_integer_32[0]; }
		};

		template<>
		struct ValueCaster<type::unsigned_integer>
		{
			constexpr static auto from(const type::unsigned_integer data) noexcept -> Value { return {.unsigned_integer_32 = {data, 0, 0, 0}}; }

			constexpr static auto to(const Value& data) noexcept -> type::unsigned_integer { return data.unsigned_integer_32[0]; }
		};

		template<>
		struct ValueCaster<type::signed_long_integer>
		{
			constexpr static auto from(const type::signed_long_integer data) noexcept -> Value { return {.signed_integer_64 = {data, 0}}; }

			constexpr static auto to(const Value& data) noexcept -> type::signed_long_integer { return data.signed_integer_64[0]; }
		};

		template<>
		struct ValueCaster<type::unsigned_long_integer>
		{
			constexpr static auto from(const type::unsigned_long_integer data) noexcept -> Value { return {.unsigned_integer_64 = {data, 0}}; }

			constexpr static auto to(const Value& data) noexcept -> type::unsigned_long_integer { return data.unsigned_integer_64[0]; }
		};
	}
}
