#pragma once

#include <cstddef>
#include <cstdint>
#include <gsl/debug/assert.hpp>

namespace gal::gsl::type
{
	template<typename T>
	class BoxedCasterImpl;

	enum class boxed_cast_policy
	{
		UNDEFINED,
		IMPLICIT,
		SPECIFIED
	};

	struct boxed_caster_undefined
	{
		constexpr static boxed_cast_policy value = boxed_cast_policy::UNDEFINED;
	};

	struct boxed_caster_implicit
	{
		constexpr static boxed_cast_policy value = boxed_cast_policy::IMPLICIT;
	};

	struct boxed_caster_specified
	{
		constexpr static boxed_cast_policy value = boxed_cast_policy::SPECIFIED;
	};

	template<typename T>
	struct is_boxed_caster_impl : std::false_type {};

	// SPECIFIED
	template<typename T>
		requires std::derived_from<BoxedCasterImpl<T>, boxed_caster_specified>
	struct is_boxed_caster_impl<BoxedCasterImpl<T>> : std::true_type {};

	// IMPLICIT
	template<typename T>
		requires std::derived_from<BoxedCasterImpl<T>, boxed_caster_implicit>
	struct is_boxed_caster_impl<BoxedCasterImpl<T>> : std::true_type {};

	// UNDEFINED
	template<typename T>
		requires std::derived_from<BoxedCasterImpl<T>, boxed_caster_undefined>
	struct is_boxed_caster_impl<BoxedCasterImpl<T>> : std::false_type { };

	template<typename T>
	constexpr static bool is_boxed_caster_impl_v = is_boxed_caster_impl<T>::value;

	namespace value_detail
	{
		union alignas(16) Value
		{
			std::int32_t signed_integer_32[4];
			std::uint32_t unsigned_integer_32[4];

			std::int64_t signed_integer_64[2];
			std::uint64_t unsigned_integer_64[2];

			float single_precision[4];
			double double_precision[2];

			char bits[16];

			struct alignas(8) pointer_type
			{
				char bits[8];
			};

			pointer_type pointers[2];

			// accessor only!
			void* raw_pointer[2];
			// accessor only!
			const void* raw_observer[2];
		};

		static_assert(sizeof(Value) == 16);

		static_assert(std::is_standard_layout_v<Value>);

		static_assert(std::is_trivially_default_constructible_v<Value>);
		static_assert(std::is_trivially_destructible_v<Value>);
		static_assert(std::is_nothrow_default_constructible_v<Value>);
		static_assert(std::is_nothrow_destructible_v<Value>);

		static_assert(std::is_trivially_copy_constructible_v<Value>);
		static_assert(std::is_trivially_copy_assignable_v<Value>);
		static_assert(std::is_nothrow_copy_constructible_v<Value>);
		static_assert(std::is_nothrow_copy_assignable_v<Value>);

		static_assert(std::is_trivially_move_constructible_v<Value>);
		static_assert(std::is_trivially_move_assignable_v<Value>);
		static_assert(std::is_nothrow_move_constructible_v<Value>);
		static_assert(std::is_nothrow_move_assignable_v<Value>);
	}

	class alignas(alignof(value_detail::Value)) BoxedValue final
	{
		template<typename>
		friend class BoxedCasterImpl;

	public:
		using value_type = value_detail::Value;

	private:
		value_type value_;

		constexpr explicit(false) BoxedValue(value_type value) noexcept
			: value_{value} {}

	public:
		constexpr explicit(false) BoxedValue() noexcept = default;

		constexpr BoxedValue(const BoxedValue&) noexcept = default;
		constexpr auto operator=(const BoxedValue&) noexcept -> BoxedValue& = default;
		constexpr BoxedValue(BoxedValue&&) noexcept = default;
		constexpr auto operator=(BoxedValue&&) noexcept -> BoxedValue& = default;
		constexpr ~BoxedValue() noexcept = default;
	};

	static_assert(sizeof(BoxedValue) == sizeof(value_detail::Value));
	static_assert(std::is_standard_layout_v<BoxedValue>);
	static_assert(std::is_trivially_destructible_v<BoxedValue>);
	// static_assert(std::is_nothrow_constructible_v<BoxedValue, BoxedValue::value_type>);
	static_assert(std::is_nothrow_default_constructible_v<BoxedValue>);
	static_assert(std::is_nothrow_destructible_v<BoxedValue>);
	static_assert(std::is_trivially_copy_constructible_v<BoxedValue>);
	static_assert(std::is_trivially_copy_assignable_v<BoxedValue>);
	static_assert(std::is_nothrow_copy_constructible_v<BoxedValue>);
	static_assert(std::is_nothrow_copy_assignable_v<BoxedValue>);
	static_assert(std::is_trivially_move_constructible_v<BoxedValue>);
	static_assert(std::is_trivially_move_assignable_v<BoxedValue>);
	static_assert(std::is_nothrow_move_constructible_v<BoxedValue>);
	static_assert(std::is_nothrow_move_assignable_v<BoxedValue>);

	// =====================
	// default caster impl
	// =====================

	template<>
	class BoxedCasterImpl<void> : public boxed_caster_specified
	{
	public:
		using value_type = void;
		using size_type = std::size_t;

		// ReSharper disable once CppParameterMayBeConstPtrOrRef
		static auto copy(BoxedValue& dest, const BoxedValue& source, const size_type size)
		{
			gsl_assert(dest.value_.raw_pointer[0], "The target location of the copy points to illegal memory. Allocate memory before copying.");
			std::memcpy(dest.value_.raw_pointer[0], source.value_.raw_observer[0], size);
		}

		// ReSharper disable once CppParameterMayBeConstPtrOrRef
		static auto move(BoxedValue& dest, const BoxedValue& source, const size_type size)
		{
			gsl_assert(dest.value_.raw_pointer[0], "The target location of the move points to illegal memory. Allocate memory before moving.");
			std::memmove(dest.value_.raw_pointer[0], source.value_.raw_observer[0], size);
		}
	};

	// bool
	template<>
	class BoxedCasterImpl<bool> : public boxed_caster_specified
	{
	public:
		using value_type = bool;

		constexpr static auto from(const value_type value) noexcept -> BoxedValue { return {{.unsigned_integer_32 = {value, 0, 0, 0}}}; }

		constexpr static auto to(const BoxedValue& value) noexcept -> value_type { return value.value_.unsigned_integer_32[0]; }

		constexpr static auto copy(BoxedValue& dest, const BoxedValue& source) noexcept -> void { dest.value_.unsigned_integer_32[0] = source.value_.unsigned_integer_32[0]; }

		constexpr static auto move(BoxedValue& dest, const BoxedValue& source) noexcept -> void { copy(dest, source); }
	};

	// int32_t
	template<>
	class BoxedCasterImpl<std::int32_t> : public boxed_caster_specified
	{
	public:
		using value_type = std::int32_t;

		constexpr static auto from(const value_type value) noexcept -> BoxedValue { return {{.signed_integer_32 = {value, 0, 0, 0}}}; }

		constexpr static auto to(const BoxedValue& value) noexcept -> value_type { return value.value_.signed_integer_32[0]; }

		constexpr static auto copy(BoxedValue& dest, const BoxedValue& source) noexcept -> void { dest.value_.signed_integer_32[0] = source.value_.signed_integer_32[0]; }

		constexpr static auto move(BoxedValue& dest, const BoxedValue& source) noexcept -> void { copy(dest, source); }
	};

	// uint32_t
	template<>
	class BoxedCasterImpl<std::uint32_t> : public boxed_caster_specified
	{
	public:
		using value_type = std::uint32_t;

		constexpr static auto from(const value_type value) noexcept -> BoxedValue { return {{.unsigned_integer_32 = {value, 0, 0, 0}}}; }

		constexpr static auto to(const BoxedValue& value) noexcept -> value_type { return value.value_.unsigned_integer_32[0]; }

		constexpr static auto copy(BoxedValue& dest, const BoxedValue& source) noexcept -> void { dest.value_.unsigned_integer_32[0] = source.value_.unsigned_integer_32[0]; }

		constexpr static auto move(BoxedValue& dest, const BoxedValue& source) noexcept -> void { copy(dest, source); }
	};

	// int64_t
	template<>
	class BoxedCasterImpl<std::int64_t> : public boxed_caster_specified
	{
	public:
		using value_type = std::int64_t;

		constexpr static auto from(const value_type value) noexcept -> BoxedValue { return {{.signed_integer_64 = {value, 0}}}; }

		constexpr static auto to(const BoxedValue& value) noexcept -> value_type { return value.value_.signed_integer_64[0]; }

		constexpr static auto copy(BoxedValue& dest, const BoxedValue& source) noexcept -> void { dest.value_.signed_integer_64[0] = source.value_.signed_integer_64[0]; }

		constexpr static auto move(BoxedValue& dest, const BoxedValue& source) noexcept -> void { copy(dest, source); }
	};

	// uint64_t
	template<>
	class BoxedCasterImpl<std::uint64_t> : public boxed_caster_specified
	{
	public:
		using value_type = std::uint64_t;

		constexpr static auto from(const value_type value) noexcept -> BoxedValue { return {{.unsigned_integer_64 = {value, 0}}}; }

		constexpr static auto to(const BoxedValue& value) noexcept -> value_type { return value.value_.unsigned_integer_64[0]; }

		constexpr static auto copy(BoxedValue& dest, const BoxedValue& source) noexcept -> void { dest.value_.unsigned_integer_64[0] = source.value_.unsigned_integer_64[0]; }

		constexpr static auto move(BoxedValue& dest, const BoxedValue& source) noexcept -> void { copy(dest, source); }
	};

	// float
	template<>
	class BoxedCasterImpl<float> : public boxed_caster_specified
	{
	public:
		using value_type = float;

		constexpr static auto from(const value_type value) noexcept -> BoxedValue { return {{.single_precision = {value, 0, 0, 0}}}; }

		constexpr static auto to(const BoxedValue& value) noexcept -> value_type { return value.value_.single_precision[0]; }

		constexpr static auto copy(BoxedValue& dest, const BoxedValue& source) noexcept -> void { dest.value_.single_precision[0] = source.value_.single_precision[0]; }

		constexpr static auto move(BoxedValue& dest, const BoxedValue& source) noexcept -> void { copy(dest, source); }
	};

	// double
	template<>
	class BoxedCasterImpl<double> : public boxed_caster_specified
	{
	public:
		using value_type = double;

		constexpr static auto from(const value_type value) noexcept -> BoxedValue { return {{.double_precision = {value, 0}}}; }

		constexpr static auto to(const BoxedValue& value) noexcept -> value_type { return value.value_.double_precision[0]; }

		constexpr static auto copy(BoxedValue& dest, const BoxedValue& source) noexcept -> void { dest.value_.double_precision[0] = source.value_.double_precision[0]; }

		constexpr static auto move(BoxedValue& dest, const BoxedValue& source) noexcept -> void { copy(dest, source); }
	};

	// pointer
	template<typename T>
	class BoxedCasterImpl<T*> : public boxed_caster_implicit
	{
		using pointer_type = value_detail::Value::pointer_type;
	public:
		using value_type = T;

		constexpr static auto from(value_type* value) -> BoxedValue { return {{.pointers = {std::bit_cast<pointer_type>(value), std::bit_cast<pointer_type>(nullptr)}}}; }

		constexpr static auto to(BoxedValue& value) -> value_type* { return static_cast<value_type*>(value.value_.raw_pointer[0]); }

		// ReSharper disable once CppParameterMayBeConstPtrOrRef
		constexpr static auto copy(BoxedValue& dest, const BoxedValue& source) noexcept -> void
		{
			if constexpr (std::is_trivially_copy_assignable_v<value_type>) { BoxedCasterImpl<void>::copy(dest, source, sizeof(value_type)); }
			else
			{
				gsl_assert(to(dest), "The target location of the copy points to illegal memory. Allocate memory before copying.");
				*to(dest) = *BoxedCasterImpl<const value_type*>::to(source);
			}
		}

		constexpr static auto move(BoxedValue& dest, const BoxedValue& source) noexcept -> void
		{
			if constexpr (std::is_trivially_move_assignable_v<value_type>) { BoxedCasterImpl<void>::move(dest, source, sizeof(value_type)); }
			else
			{
				gsl_assert(to(dest), "The target location of the move points to illegal memory. Allocate memory before moving.");
				// todo: just exchange the pointer?
				dest.value_.pointers[0] = std::exchange(source.value_.pointers[0], {});
			}
		}
	};

	// const pointer
	template<typename T>
	class BoxedCasterImpl<const T*> : public boxed_caster_implicit
	{
		using pointer_type = value_detail::Value::pointer_type;

	public:
		using value_type = T;

		constexpr static auto from(const value_type* value) -> BoxedValue { return {{.pointers = {std::bit_cast<pointer_type>(value), std::bit_cast<pointer_type>(nullptr)}}}; }

		constexpr static auto to(const BoxedValue& value) -> const value_type* { return static_cast<const value_type*>(value.value_.raw_observer[0]); }

		// ReSharper disable once CppParameterMayBeConstPtrOrRef
		constexpr static auto copy(BoxedValue& dest, const BoxedValue& source) noexcept -> void
		{
			if constexpr (std::is_trivially_copy_assignable_v<value_type>) { BoxedCasterImpl<void>::copy(dest, source, sizeof(value_type)); }
			else
			{
				gsl_assert(BoxedCasterImpl<value_type*>::to(dest), "The target location of the copy points to illegal memory. Allocate memory before copying.");
				*BoxedCasterImpl<value_type*>::to(dest) = *to(source);
			}
		}

		// move is not allowed
	};

	// reference
	template<typename T>
	class BoxedCasterImpl<T&> : public boxed_caster_implicit
	{
		using pointer_type = value_detail::Value::pointer_type;

	public:
		using value_type = T;

		constexpr static auto from(value_type& value) -> BoxedValue { return {{.pointers = {std::bit_cast<pointer_type>(std::addressof(value)), std::bit_cast<pointer_type>(nullptr)}}}; }

		constexpr static auto to(const BoxedValue& value) -> value_type& { return *static_cast<value_type*>(value.value_.raw_pointer[0]); }

		// ReSharper disable once CppParameterMayBeConstPtrOrRef
		constexpr static auto copy(BoxedValue& dest, const BoxedValue& source) noexcept -> void
		{
			if constexpr (std::is_trivially_copy_assignable_v<value_type>) { BoxedCasterImpl<void>::copy(dest, source, sizeof(value_type)); }
			else { to(dest) = BoxedCasterImpl<const T&>::to(source); }
		}

		constexpr static auto move(BoxedValue& dest, const BoxedValue& source) noexcept -> void
		{
			if constexpr (std::is_trivially_move_assignable_v<value_type>) { BoxedCasterImpl<void>::move(dest, source, sizeof(value_type)); }
			else { to(dest) = std::move(to(source)); }
		}
	};

	// const reference
	template<typename T>
	class BoxedCasterImpl<const T&> : public boxed_caster_implicit
	{
		using pointer_type = value_detail::Value::pointer_type;

	public:
		using value_type = T;

		constexpr static auto from(const value_type& value) -> BoxedValue { return {{.pointers = {std::bit_cast<pointer_type>(std::addressof(value)), std::bit_cast<pointer_type>(nullptr)}}}; }

		constexpr static auto to(const BoxedValue& value) -> const value_type& { return *static_cast<const value_type*>(value.value_.raw_observer[0]); }

		// ReSharper disable once CppParameterMayBeConstPtrOrRef
		constexpr static auto copy(BoxedValue& dest, const BoxedValue& source) noexcept -> void
		{
			if constexpr (std::is_trivially_copy_assignable_v<value_type>) { BoxedCasterImpl<void>::copy(dest, source, sizeof(value_type)); }
			else { to(dest) = BoxedCasterImpl<const T&>::to(source); }
		}

		// move is not allowed
	};

	class BoxedCaster final
	{
	public:
		template<typename T>
			requires is_boxed_caster_impl_v<BoxedCasterImpl<T>>
		[[nodiscard]] constexpr static auto from(const BoxedCasterImpl<T>& impl, T&& data) noexcept(noexcept(impl.from(std::forward<T>(data)))) -> BoxedValue { return impl.from(std::forward<T>(data)); }

		template<typename T>
			requires is_boxed_caster_impl_v<BoxedCasterImpl<T>>
		[[nodiscard]] constexpr static auto from(T&& data) noexcept(noexcept(BoxedCasterImpl<T>::from(std::forward<T>(data)))) -> BoxedValue { return BoxedCasterImpl<T>::from(std::forward<T>(data)); }

		template<typename T>
			requires(!is_boxed_caster_impl_v<BoxedCasterImpl<T>>)
		[[nodiscard]] constexpr static auto from(const BoxedCasterImpl<T>& impl, T&& data) = delete;

		template<typename T>
			requires(!is_boxed_caster_impl_v<BoxedCasterImpl<T>>)
		[[nodiscard]] constexpr static auto from(T&& data) = delete;

		template<typename T>
			requires is_boxed_caster_impl_v<BoxedCasterImpl<T>>
		[[nodiscard]] constexpr static auto to(const BoxedCasterImpl<T>& impl, BoxedValue& data) noexcept(noexcept(impl.to(data))) -> decltype(auto) { return impl.to(data); }

		template<typename T>
			requires is_boxed_caster_impl_v<BoxedCasterImpl<T>>
		[[nodiscard]] constexpr static auto to(BoxedValue& data) noexcept(noexcept(BoxedCasterImpl<T>::to(data))) -> decltype(auto) { return BoxedCasterImpl<T>::to(data); }

		template<typename T>
			requires(!is_boxed_caster_impl_v<BoxedCasterImpl<T>>)
		[[nodiscard]] constexpr static auto to(const BoxedCasterImpl<T>& impl, BoxedValue& data) = delete;

		template<typename T>
			requires(!is_boxed_caster_impl_v<BoxedCasterImpl<T>>)
		[[nodiscard]] constexpr static auto to(BoxedValue& data) = delete;

		template<typename T>
			requires is_boxed_caster_impl_v<BoxedCasterImpl<T>>
		constexpr static auto copy(const BoxedCasterImpl<T>& impl, BoxedValue& dest, const BoxedValue& source) noexcept(noexcept(impl.copy(dest, source))) -> void { impl.copy(dest, source); }

		template<typename T>
			requires is_boxed_caster_impl_v<BoxedCasterImpl<T>>
		constexpr static auto copy(BoxedValue& dest, const BoxedValue& source) noexcept(noexcept(BoxedCasterImpl<T>::copy(dest, source))) -> void { BoxedCasterImpl<T>::copy(dest, source); }

		static auto copy(BoxedValue& dest, const BoxedValue& source, const BoxedCasterImpl<void>::size_type size) -> void { BoxedCasterImpl<void>::copy(dest, source, size); }

		template<typename T>
			requires(!is_boxed_caster_impl_v<BoxedCasterImpl<T>>)
		constexpr static auto copy(const BoxedCasterImpl<T>& impl, BoxedValue& dest, const BoxedValue& source) = delete;

		template<typename T>
			requires(!is_boxed_caster_impl_v<BoxedCasterImpl<T>>)
		constexpr static auto copy(BoxedValue& dest, const BoxedValue& source) = delete;

		template<typename T>
			requires is_boxed_caster_impl_v<BoxedCasterImpl<T>>
		constexpr static auto move(const BoxedCasterImpl<T>& impl, BoxedValue& dest, const BoxedValue& source) noexcept(noexcept(impl.move(dest, source))) -> void { impl.move(dest, source); }

		template<typename T>
			requires is_boxed_caster_impl_v<BoxedCasterImpl<T>>
		constexpr static auto move(BoxedValue& dest, const BoxedValue& source) noexcept(noexcept(BoxedCasterImpl<T>::move(dest, source))) -> void { BoxedCasterImpl<T>::move(dest, source); }

		static auto move(BoxedValue& dest, const BoxedValue& source, const BoxedCasterImpl<void>::size_type size) -> void { BoxedCasterImpl<void>::move(dest, source, size); }

		template<typename T>
			requires(!is_boxed_caster_impl_v<BoxedCasterImpl<T>>)
		[[nodiscard]] constexpr static auto move(const BoxedCasterImpl<T>& impl, BoxedValue& dest, const BoxedValue& source) = delete;

		template<typename T>
			requires(!is_boxed_caster_impl_v<BoxedCasterImpl<T>>)
		[[nodiscard]] constexpr static auto move(BoxedValue& dest, const BoxedValue& source) = delete;
	};
}// namespace gal::gsl::type
