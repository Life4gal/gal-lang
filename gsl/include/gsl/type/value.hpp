#pragma once

#include <cstddef>
#include <cstdint>

namespace gal::gsl::type
{
	template<typename T>
	class ValueCaster;

	class Value
	{
	public:
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
		[[nodiscard]] constexpr auto
		as() const -> decltype(auto) { return ValueCaster<T>::to(*this); }
	};

	static_assert(sizeof(Value) == sizeof(std::uint32_t) * 4);
	static_assert(sizeof(Value) == sizeof(float) * 4);

	enum class value_caster_policy
	{
		UNDEFINED,
		IMPLICIT,
		SPECIFIED
	};

	struct value_caster_undefined
	{
		constexpr static value_caster_policy value = value_caster_policy::UNDEFINED;
	};

	struct value_caster_implicit
	{
		constexpr static value_caster_policy value = value_caster_policy::IMPLICIT;
	};

	struct value_caster_specified
	{
		constexpr static value_caster_policy value = value_caster_policy::SPECIFIED;
	};

	template<typename T>
	class ValueCaster : public value_caster_undefined
	{
	public:
		constexpr static auto from(const T&) -> Value = delete;
		constexpr static auto to(const Value&) -> decltype(auto) = delete;
	};

	template<typename T>
	class ValueCaster<const T> : public ValueCaster<T> { };

	template<typename T>
	class ValueCaster<T*> : public value_caster_implicit
	{
	public:
		constexpr static auto from(const T* data) -> Value { return Value{.raw_observer = data}; }

		constexpr static auto to(const Value& data) -> decltype(auto)
		{
			// not checked
			return static_cast<T*>(data.raw_observer);
		}
	};

	template<typename T>
	class ValueCaster<const T*> : public value_caster_implicit
	{
	public:
		constexpr static auto from(const T* data) -> Value { return Value{.raw_observer = data}; }

		constexpr static auto to(const Value& data) -> decltype(auto)
		{
			// not checked
			return static_cast<const T*>(data.raw_observer);
		}
	};

	template<typename T>
	class ValueCaster<T&> : public value_caster_implicit
	{
	public:
		constexpr static auto from(const T& data) -> Value { return Value{.raw_observer = &data}; }

		constexpr static auto to(const Value& data) -> decltype(auto)
		{
			// not checked
			return *static_cast<T*>(data.raw_observer);
		}
	};

	template<typename T>
	class ValueCaster<const T&> : public value_caster_implicit
	{
	public:
		constexpr static auto from(const T& data) -> Value { return Value{.raw_observer = &data}; }

		constexpr static auto to(const Value& data) -> decltype(auto)
		{
			// not checked
			return *static_cast<const T*>(data.raw_observer);
		}
	};

	// Explicitly instantiate some commonly used transformations
	template<>
	class ValueCaster<std::int64_t> : public value_caster_specified
	{
	public:
		constexpr static auto from(const std::int64_t data) -> Value { return Value{.signed_integer_64 = {data, 0}}; }

		constexpr static auto to(const Value& data) -> decltype(auto) { return data.signed_integer_64[0]; }
	};

	template<>
	class ValueCaster<double> : public value_caster_specified
	{
	public:
		constexpr static auto from(const double data) -> Value { return Value{.double_precision = {data, 0}}; }

		constexpr static auto to(const Value& data) -> decltype(auto) { return data.double_precision[0]; }
	};
}// namespace gal::gsl::core
