#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace gal::gsl::core
{
	template<typename T>
	struct ValueCaster;

	struct Value
	{
		union alignas(alignof(std::max_align_t))
		{
			char bits[16];
			void* raw_pointer;
			const void* raw_observer;
			std::int32_t signed_integer_32[4];
			std::uint32_t unsigned_integer_32[4];
			std::int64_t signed_integer_64[2];
			std::uint64_t unsigned_integer_64[2];
			float single_precision[4];
			double double_precision[2];
		};

		template<typename T>
			requires ValueCaster<T>::value
		[[nodiscard]] constexpr auto as() const -> decltype(auto) { return ValueCaster<T>::to(*this); }
	};

	static_assert(sizeof(Value) == sizeof(std::uint32_t) * 4);
	static_assert(sizeof(Value) == sizeof(float) * 4);

	template<typename T>
	struct ValueCaster : std::false_type
	{
		constexpr static auto from(const T&) -> Value = delete;
		constexpr static auto to(const Value&) -> decltype(auto) = delete;
	};
}
