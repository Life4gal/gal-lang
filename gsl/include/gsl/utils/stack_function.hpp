#pragma once

namespace gal::gsl::utils
{
	/**
	 * \brief Wraps around lambda, which is passed on the stack, make lambda with capture work like function pointer, and only exists at eval time.
	 * \tparam Function signature of function
	 *
	 * \code
	 * foo(StackFunction([&](args...) { lambda body... })
	 * \endcode
	 */
	template<typename Function>
	class StackFunction;

	template<typename T>
	struct is_stack_function : std::false_type {};

	template<typename Function>
	struct is_stack_function<StackFunction<Function>> : std::true_type {};

	template<typename T>
	constexpr static bool is_stack_function_v = is_stack_function<T>::value;

	template<typename Return, typename... Args>
	class StackFunction<Return(Args ...)>
	{
	public:
		using return_type = Return;
		using arguments_type = std::tuple<Args...>;
		using address_type = const char*;

		using signature_type = return_type(*)(address_type, Args ...);

		template<typename Functor>
		[[nodiscard]] constexpr static auto invoker(Functor* target, Args&&... args) -> return_type { return std::invoke(*target, std::forward<Args>(args)...); }

	private:
		address_type target_address_;
		signature_type signature_;

	public:
		constexpr StackFunction() noexcept
			: target_address_{nullptr},
			signature_{nullptr} {}

		template<typename Functor>
			requires(!is_stack_function_v<Functor>)
		constexpr explicit(false) StackFunction(const Functor& functor) noexcept
			: target_address_{reinterpret_cast<address_type>(&functor)},
			signature_{reinterpret_cast<signature_type>(invoker<Functor>)} {}

		constexpr StackFunction(const StackFunction& other) = delete;
		constexpr StackFunction(StackFunction&& other) noexcept = delete;
		constexpr auto operator=(const StackFunction& other) -> StackFunction& = delete;
		constexpr auto operator=(StackFunction&& other) noexcept -> StackFunction& = delete;

		constexpr ~StackFunction() noexcept = default;

		[[nodiscard]] constexpr auto empty() const noexcept -> bool { return target_address_ == nullptr; }

		[[nodiscard]] constexpr explicit operator bool() const noexcept { return !empty(); }

		constexpr auto operator()(Args&&... args) const -> return_type { return signature_(target_address_, std::forward<Args>(args)...); }
	};
}
