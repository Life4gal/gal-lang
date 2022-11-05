// #pragma once
//
// #include <gsl/string/string.hpp>
// #include <gsl/string/string_view.hpp>
// #include <gsl/container/vector.hpp>
//
// #include <variant>
//
// namespace gal::gsl::frontend
// {
// 	using symbol_name = string::string;
// 	using symbol_name_view = std::string_view;
//
// 	struct node_comment
// 	{
// 		using value_type = symbol_name;
//
// 		value_type comment;
// 	};
//
// 	struct node_boolean
// 	{
// 		using value_type = bool;
//
// 		value_type boolean;
// 	};
//
// 	struct node_number
// 	{
// 		using value_type = symbol_name;
//
// 		// The actual type will be selected according to the length
// 		value_type value;
// 	};
//
// 	struct node_string
// 	{
// 		using value_type = symbol_name;
//
// 		value_type string;
// 	};
//
// 	struct unary_expression;
// 	struct binary_expression;
//
// 	class Node
// 	{
// 	public:
// 		using value_type = std::variant<
// 			// node_comment,
// 			node_boolean,
// 			node_number,
// 			node_string,
// 			unary_expression,
// 			binary_expression
// 		>;
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
// 	};
//
// 	using node_tree_type = container::vector<Node>;
//
// 	struct unary_expression
// 	{
// 		enum class operand_type
// 		{
// 			INVALID,
//
// 			// -x
// 			NEGATION,
// 			// ~x
// 			BIT_COMPLEMENT,
// 		};
//
// 		operand_type type{operand_type::INVALID};
// 		Node node;
// 	};
//
// 	struct binary_expression
// 	{
// 		enum class operand_type
// 		{
// 			INVALID,
//
// 			// x ** y
// 			POW,
//
// 			// x * y
// 			MULTIPLY,
// 			// x / y
// 			DIVIDE,
// 			// x + y
// 			PLUS,
// 			// x - y
// 			MINUS,
//
// 			// x & y
// 			BIT_AND,
// 			// x | y
// 			BIT_OR,
// 			// x ^ y
// 			BIT_XOR,
// 		};
//
// 		operand_type type{operand_type::INVALID};
// 		Node left;
// 		Node right;
// 	};
// }
