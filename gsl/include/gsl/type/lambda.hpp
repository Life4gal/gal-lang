#pragma once


namespace gal::gsl
{
	namespace type
	{
		class Lambda
		{
			friend core::ValueCaster<Lambda>;
		public:
			using data_type = void*;

		private:
			data_type capture_;

		public:
			explicit Lambda(const data_type capture = nullptr)
				: capture_{capture} {}

			[[nodiscard]] constexpr auto operator==(const Lambda& other) const noexcept -> bool { return capture_ == other.capture_; }

			[[nodiscard]] constexpr auto operator==(const void* capture) const noexcept -> bool { return capture_ == capture; }

			[[nodiscard]] constexpr explicit(true) operator bool() const noexcept { return capture_; }
		};

		static_assert(sizeof(Lambda) == sizeof(Lambda::data_type));

		template<typename Return, typename... Args>
		class CallableLambda : public Lambda
		{
		public:
			using return_type = Return;
			using arguments_type = std::tuple<Args...>;

			using Lambda::Lambda;

			// todo
		};
	}

	namespace core
	{
		template<>
		class ValueCaster<type::Lambda> : public value_caster_implicit
		{
		public:
			constexpr static auto from(const type::Lambda& data) noexcept -> Value { return ValueCaster<void*>::from(data.capture_); }

			constexpr static auto to(const Value& data) noexcept -> type::Lambda { return type::Lambda{ValueCaster<void*>::to(data)}; }
		};

		template<typename Return, typename... Args>
		class ValueCaster<type::CallableLambda<Return, Args...>> : public ValueCaster<type::Lambda> { };
	}
}
