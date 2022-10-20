#pragma once

#include <gsl/core/value.hpp>
#include <gsl/type/type_descriptor.hpp>

namespace gal::gsl
{
	namespace core
	{
		class ModuleFunction;
	}

	namespace type
	{
		class Function
		{
			friend core::ValueCaster<Function>;
		public:
		private:
			core::ModuleFunction* function_;

		public:
			Function()
				: function_{nullptr} {}

			explicit Function(core::ModuleFunction* function)
				: function_{function} {}

			[[nodiscard]] constexpr auto operator==(const Function& other) const noexcept -> bool { return function_ == other.function_; }

			[[nodiscard]] constexpr auto operator==(const void* function) const noexcept -> bool { return function_ == function; }

			[[nodiscard]] constexpr explicit(true) operator bool() const noexcept { return function_; }
		};

		static_assert(sizeof(Function) == sizeof(core::ModuleNode*));

		template<typename Return, typename... Args>
		class CallableFunction : public Function
		{
		public:
			using return_type = Return;
			using arguments_type = std::tuple<Args...>;

			using Function::Function;

			// todo
		};
	}

	namespace core
	{
		template<>
		class ValueCaster<type::Function> : public value_caster_implicit
		{
		public:
			constexpr static auto from(const type::Function& data) noexcept -> Value { return ValueCaster<ModuleFunction*>::from(data.function_); }

			constexpr static auto to(const Value& data) noexcept -> type::Function { return type::Function{ValueCaster<ModuleFunction*>::to(data)}; }
		};

		template<typename Return, typename... Args>
		class ValueCaster<type::CallableFunction<Return, Args...>> : public ValueCaster<type::Function> { };
	}// namespace core
}
