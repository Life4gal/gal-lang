// #pragma once
//
// #include <gsl/string/string.hpp>
// #include <gsl/string/string_view.hpp>
// #include <gsl/container/vector.hpp>
// #include <optional>
// #include <variant>
// #include <memory>
//
// namespace gal::gsl::parse
// {
// 	struct node_comment
// 	{
// 		using value_type = string::string;
//
// 		value_type value;
//
// 		template<typename Function>
// 		auto invoke(Function&& function) const -> void { std::invoke(std::forward<Function>(function), value); }
// 	};
//
// 	struct node_bool
// 	{
// 		bool value;
//
// 		template<typename Function>
// 		auto invoke(Function&& function) const -> void { std::invoke(std::forward<Function>(function), value); }
// 	};
//
// 	// The actual type will be selected according to the length
// 	struct node_number
// 	{
// 		using value_type = string::string;
// 		// using value_type = std::uint64_t;
// 		using fraction_type = string::string;
// 		using exponent_type = std::int16_t;
//
// 		value_type value;
// 		std::optional<fraction_type> fraction;
// 		std::optional<std::int16_t> exponent;
//
// 		template<typename Function>
// 		auto invoke(Function&& function) const -> void
// 		{
// 			std::invoke(std::forward<Function>(function), value);
// 			if (fraction.has_value()) { std::invoke(std::forward<Function>(function), *fraction); }
//
// 			if (exponent.has_value()) { std::invoke(std::forward<Function>(function), *exponent); }
// 		}
// 	};
//
// 	struct node_string
// 	{
// 		using value_type = string::string;
//
// 		value_type value;
//
// 		template<typename Function>
// 		auto invoke(Function&& function) const -> void { std::invoke(std::forward<Function>(function), value); }
// 	};
//
//
// 	struct node_expression
// 	{
// 		// todo
// 		enum class operand_type
// 		{
// 			// unary_arithmetic
// 			NEGATION,
// 			BIT_COMPLEMENT,
// 			// binary_arithmetic
// 			POW,
// 			MULTIPLY,
// 			DIVIDE,
// 			PLUS,
// 			MINUS,
// 			BIT_AND,
// 			BIT_OR,
// 			BIT_XOR,
// 			// comparison
// 			EQUAL,
// 			LESS_EQUAL,
// 			LESS,
// 			GREATER_EQUAL,
// 			GREATER,
// 		};
//
// 		template<typename Function>
// 		auto invoke(Function&& function) const -> void
// 		{
// 			(void)function;
// 			return;
// 		}
// 	};
//
// 	template<typename Expression = node_expression>
// 	using node_expression_ptr = std::unique_ptr<Expression>;
//
// 	template<typename Expression = node_expression, typename... Args>
// 	[[nodiscard]] constexpr auto make_expression(Args&&... args) -> node_expression_ptr<Expression> { return std::make_unique<Expression>(std::forward<Args>(args)...); }
//
// 	struct node_expression_literal : public node_expression
// 	{
// 		// todo
// 		using value_type = node_number;
//
// 		value_type value;
// 	};
//
// 	struct node_expression_identifier : public node_expression
// 	{
// 		using value_type = node_string::value_type;
//
// 		value_type value;
// 	};
//
// 	struct node_expression_unary_arithmetic : public node_expression
// 	{
// 		enum class operand_type
// 		{
// 			NEGATION,
// 			BIT_COMPLEMENT,
// 		};
// 	};
//
// 	struct node_expression_binary_arithmetic : public node_expression
// 	{
// 		enum class operand_type
// 		{
// 			POW,
// 			MULTIPLY,
// 			DIVIDE,
// 			PLUS,
// 			MINUS,
// 			BIT_AND,
// 			BIT_OR,
// 			BIT_XOR
// 		};
//
// 		operand_type operand;
// 		node_expression_ptr<> lhs;
// 		node_expression_ptr<> rhs;
// 	};
//
// 	struct node_expression_comparison : public node_expression
// 	{
// 		enum class operand_type
// 		{
// 			EQUAL,
// 			LESS_EQUAL,
// 			LESS,
// 			GREATER_EQUAL,
// 			GREATER,
// 		};
// 	};
//
// 	struct node_expression_composite : public node_expression
// 	{
// 		std::vector<node_expression_ptr<>> operands;
//
// 		auto push_operand(node_expression_ptr<>&& operand) -> void { operands.push_back(std::move(operand)); }
// 	};
//
// 	struct node_expression_assignment : public node_expression
// 	{
// 		node_expression_ptr<> lhs;
// 		node_expression_ptr<> rhs;
// 	};
//
// 	class Node
// 	{
// 	public:
// 		using value_type = std::variant<
// 			node_comment,
// 			node_bool,
// 			node_number,
// 			node_string,
// 			node_expression_ptr<>>;
//
// 	private:
// 		value_type value_;
//
// 	public:
// 		// todo: allow invalid state?
// 		Node() = delete;
//
// 		template<typename T>
// 			requires std::is_constructible_v<value_type, T>
// 		explicit(false) Node(T&& arg)
// 			: value_{std::forward<T>(arg)} {}
//
// 		// test
// 		template<typename Function>
// 		auto invoke(Function&& function) const -> void
// 		{
// 			std::visit(
// 					[f = std::forward<Function>(function)](const auto& value) mutable
// 					{
// 						if constexpr (requires { value.operator*(); }) { value->invoke(std::forward<Function>(f)); }
// 						else { value.invoke(std::forward<Function>(f)); }
// 					},
// 					value_);
// 		}
// 	};
//
// 	using node_tree_result = container::vector<Node>;
//
// 	[[nodiscard]] auto parse_file(string::string_view filename) -> node_tree_result;
// }
