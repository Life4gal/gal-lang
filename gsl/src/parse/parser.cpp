// #include <gsl/parse/parser.hpp>
//
// #include <lexy/action/parse.hpp> // lexy::parse
// #include <lexy/callback.hpp>     // value callbacks
// #include <lexy/dsl.hpp>          // lexy::dsl::*
// #include <lexy/input/file.hpp>   // lexy::read_file
// #include <lexy_ext/report_error.hpp> // lexy_ext::report_error
//
// #include <exception>
// #include <iostream>
//
// namespace
// {
// 	using namespace gal::gsl;
// 	namespace dsl = lexy::dsl;
//
// 	namespace parser
// 	{
// 		struct comment : public lexy::token_production
// 		{
// 			struct single_line : public lexy::transparent_production
// 			{
// 				constexpr static auto rule = dsl::delimited(dsl::lit_c<'#'>, dsl::newline)(dsl::code_point);
// 				constexpr static auto value = lexy::as_string<parse::node_comment::value_type, lexy::utf8_encoding>;
// 			};
//
// 			struct multi_line_single_quotation : public lexy::transparent_production
// 			{
// 				constexpr static auto rule = dsl::delimited(LEXY_LIT("'''"), LEXY_LIT("'''"))(dsl::code_point);
// 				constexpr static auto value = lexy::as_string<parse::node_comment::value_type, lexy::utf8_encoding>;
// 			};
//
// 			struct multi_line_double_quotation : public lexy::transparent_production
// 			{
// 				constexpr static auto rule = dsl::delimited(LEXY_LIT("\"\"\""), LEXY_LIT("\"\"\""))(dsl::code_point);
// 				constexpr static auto value = lexy::as_string<parse::node_comment::value_type, lexy::utf8_encoding>;
// 			};
//
// 			constexpr static auto rule = dsl::p<single_line> | dsl::p<multi_line_single_quotation> | dsl::p<multi_line_double_quotation>;
//
// 			constexpr static auto value = lexy::construct<parse::node_comment>;
// 		};
//
// 		struct boolean : public lexy::token_production
// 		{
// 			struct true_part : lexy::transparent_production
// 			{
// 				constexpr static auto rule = LEXY_LIT("true");
// 				constexpr static auto value = lexy::constant(true);
// 			};
//
// 			struct false_part : lexy::transparent_production
// 			{
// 				constexpr static auto rule = LEXY_LIT("false");
// 				constexpr static auto value = lexy::constant(false);
// 			};
//
// 			constexpr static auto rule = dsl::p<true_part> | dsl::p<false_part>;
// 			constexpr static auto value = lexy::construct<parse::node_bool>;
// 		};
//
// 		struct number : public lexy::token_production
// 		{
// 			struct value_part : lexy::transparent_production
// 			{
// 				struct invalid_digit
// 				{
// 					consteval static auto name() { return "invalid floating point digit"; }
// 				};
//
// 				constexpr static auto rule = []
// 				{
// 					// todo: sign
// 					constexpr auto optional_sign = dsl::sign;
// 					constexpr auto digits = dsl::digit<>.error<invalid_digit>;
//
// 					return dsl::identifier(digits);
// 				}();
//
// 				constexpr static auto value = lexy::as_string<parse::node_number::value_type, lexy::utf8_encoding>;
// 			};
//
// 			struct fraction_part : lexy::transparent_production
// 			{
// 				constexpr static auto rule = dsl::lit_c<'.'> >> dsl::capture(dsl::digits<>);
//
// 				constexpr static auto value = lexy::as_string<parse::node_number::fraction_type, lexy::utf8_encoding>;
// 			};
//
// 			struct exponent_part : lexy::transparent_production
// 			{
// 				constexpr static auto rule = (dsl::lit_c<'e'> | dsl::lit_c<'E'>) >> dsl::sign + dsl::integer<parse::node_number::exponent_type>;
//
// 				constexpr static auto value = lexy::as_integer<parse::node_number::exponent_type>;
// 			};
//
// 			// peek '-' or digit
// 			// + optional fraction_part and exponent_part
// 			constexpr static auto rule = dsl::peek(dsl::lit_c<'-'> / dsl::digit<>) >> dsl::p<value_part> + dsl::opt(dsl::p<fraction_part>) + dsl::opt(dsl::p<exponent_part>);
//
// 			constexpr static auto value = lexy::construct<parse::node_number>;
// 		};
//
// 		struct string : public lexy::token_production
// 		{
// 			struct value_part : public lexy::transparent_production
// 			{
// 				struct invalid_character
// 				{
// 					consteval static auto name() noexcept { return "invalid character in string literal"; }
// 				};
//
// 				// A mapping of the simple escape sequences to their replacement values.
// 				constexpr static auto escaped_symbols = lexy::symbol_table<char>//
// 														.map<'"'>('"')
// 														.map<'\\'>('\\')
// 														.map<'/'>('/')
// 														.map<'b'>('\b')
// 														.map<'f'>('\f')
// 														.map<'n'>('\n')
// 														.map<'r'>('\r')
// 														.map<'t'>('\t');
//
// 				// Unicode code point can be specified by its encoding in UTF-16:
// 				// * code points <= 0xFFFF are specified using `\uNNNN`.
// 				// * other code points are specified by two surrogate UTF-16 sequences.
// 				// However, we don't combine the two surrogates into the final code point,
// 				// instead keep them separate and require a later pass that merges them if necessary.
// 				// (This behavior is allowed by the standard).
// 				struct code_point_id
// 				{
// 					// We parse the integer value of a UTF-16 code unit.
// 					constexpr static auto rule = LEXY_LIT("u") >> dsl::code_unit_id<lexy::utf16_encoding, 4>;
// 					// And convert it into a code point, which might be a surrogate.
// 					constexpr static auto value = lexy::construct<lexy::code_point>;
// 				};
//
// 				constexpr static auto rule = []
// 				{
// 					// Everything is allowed inside a string except for control characters.
// 					constexpr auto code_point = (-dsl::unicode::control).error<invalid_character>;
//
// 					// Escape sequences start with a backlash and either map one of the symbols,
// 					// or a Unicode code point.
// 					constexpr auto escape = dsl::backslash_escape.symbol<escaped_symbols>().rule(dsl::p<code_point_id>);
//
// 					// String of code_point with specified escape sequences, surrounded by ".
// 					// We abort string parsing if we see a `newline` to handle missing closing ".
// 					// todo: allow multi-line string
// 					return dsl::quoted.limit(dsl::ascii::newline)(code_point, escape);
// 				}();
//
// 				constexpr static auto value = lexy::as_string<parse::node_string::value_type, lexy::utf8_encoding>;
// 			};
//
// 			constexpr static auto rule = dsl::p<value_part>;
//
// 			constexpr static auto value = lexy::construct<parse::node_string>;
// 		};
//
// 		struct expression : public lexy::expression_production
// 		{
// 			struct expected_operand
// 			{
// 				constexpr static auto name = "expected operand";
// 			};
//
// 			// todo
// 			using literal = number;
//
// 			struct identifier : public lexy::transparent_production
// 			{
// 				struct value_part : public lexy::transparent_production
// 				{
// 					constexpr static auto rule = dsl::identifier(dsl::unicode::xid_start_underscore, dsl::unicode::xid_continue);
//
// 					constexpr static auto value = lexy::as_string<parse::node_expression_identifier::value_type, lexy::utf8_encoding>;
// 				};
//
// 				constexpr static auto rule = dsl::p<value_part>;
//
// 				constexpr static auto value = lexy::construct<parse::node_expression_identifier>;
// 			};
//
// 			// An expression that is nested inside another expression.
// 			struct nested_expression : public lexy::transparent_production
// 			{
// 				// We change the whitespace rule to allow newlines:
// 				// as it's nested, the REPL can properly handle continuation lines.
// 				constexpr static auto whitespace = dsl::ascii::space | (dsl::backslash >> dsl::newline);
//
// 				// The rule itself just recurse back to expression, but with the adjusted whitespace now.
// 				constexpr static auto rule = dsl::recurse<expression>;
// 				// see `value` below
// 				constexpr static auto value = lexy::forward<parse::node_expression_ptr<>>;
// 			};
//
// 			// x ** n
// 			struct math_power : public dsl::infix_op_right
// 			{
// 				constexpr static auto op = dsl::op<parse::node_expression_binary_arithmetic::operand_type::POW>(LEXY_LIT("**"));
//
// 				// math_power has highest binding power, so it's operand is the atom rule.
// 				using operand = dsl::atom;
// 			};
//
// 			// -x
// 			struct math_negation : public dsl::prefix_op
// 			{
// 				constexpr static auto op = dsl::op<parse::node_expression_unary_arithmetic::operand_type::NEGATION>(LEXY_LIT("-"));
//
// 				using operand = math_power;
// 			};
//
// 			// ~x
// 			struct math_complement : public dsl::prefix_op
// 			{
// 				constexpr static auto op = dsl::op<parse::node_expression_unary_arithmetic::operand_type::BIT_COMPLEMENT>(LEXY_LIT("~"));
//
// 				using operand = dsl::atom;
// 			};
//
// 			// x * x
// 			struct math_product : public dsl::infix_op_left
// 			{
// 				constexpr static auto op = []
// 				{
// 					// Since we have both ** and * as possible operators, we need to ensure that * is only
// 					// matched when it is not followed by *.
// 					// In the particular situation where ** has higher binding power, it doesn't actually
// 					// matter here, but it's better to be more resilient.
// 					constexpr auto single_star = dsl::not_followed_by(LEXY_LIT("*"), LEXY_LIT("*"));
// 					return dsl::op<parse::node_expression_binary_arithmetic::operand_type::MULTIPLY>(single_star);
// 				};
//
// 				using operand = math_negation;
// 			};
//
// 			// x / x
// 			struct math_quotient : public dsl::infix_op_left
// 			{
// 				constexpr static auto op = dsl::op<parse::node_expression_binary_arithmetic::operand_type::DIVIDE>(LEXY_LIT("/"));
//
// 				using operand = math_negation;
// 			};
//
// 			// x + x or x - x
// 			struct math_plus_or_minus : public dsl::infix_op_left
// 			{
// 				constexpr static auto op = dsl::op<parse::node_expression_binary_arithmetic::operand_type::PLUS>(LEXY_LIT("+")) / dsl::op<parse::node_expression_binary_arithmetic::operand_type::MINUS>(LEXY_LIT("-"));
//
// 				using operand = math_product;
// 			};
//
// 			// x & x or x |x or x ^ x
// 			struct math_bit_operand : public dsl::infix_op_left
// 			{
// 				constexpr static auto op = dsl::op<parse::node_expression_binary_arithmetic::operand_type::BIT_AND>(LEXY_LIT("&")) /
// 											dsl::op<parse::node_expression_binary_arithmetic::operand_type::BIT_OR>(LEXY_LIT("|")) /
// 											dsl::op<parse::node_expression_binary_arithmetic::operand_type::BIT_XOR>(LEXY_LIT("^"));
//
// 				using operand = math_complement;
// 			};
//
// 			// Comparisons are list operators, which allows implementation of chaining.
// 			// x == y < z
// 			struct comparison : public dsl::infix_op_list
// 			{
// 				constexpr static auto op = dsl::op<parse::node_expression_comparison::operand_type::EQUAL>(LEXY_LIT("==")) /
// 											dsl::op<parse::node_expression_comparison::operand_type::LESS_EQUAL>(dsl::followed_by(LEXY_LIT("<"), LEXY_LIT("="))) /
// 											dsl::op<parse::node_expression_comparison::operand_type::LESS>(LEXY_LIT("<")) /
// 											dsl::op<parse::node_expression_comparison::operand_type::GREATER_EQUAL>(dsl::followed_by(LEXY_LIT(">"), LEXY_LIT("="))) /
// 											dsl::op<parse::node_expression_comparison::operand_type::GREATER>(LEXY_LIT(">"));
//
// 				// The use of dsl::groups ensures that an expression can either contain math or bit
// 				// operators. Mixing requires parenthesis.
// 				using operand = dsl::groups<math_plus_or_minus, math_bit_operand>;
// 			};
//
// 			// x ? y : z
// 			struct condition : public dsl::infix_op_single
// 			{
// 				// We treat a conditional operator, which has three operands,
// 				// as a binary operator where the operator consists of `?`, the inner operator, and `:`.
// 				// The <void> ensures that `dsl::op` does not produce a value.
// 				constexpr static auto op = dsl::op<void>(LEXY_LIT("?")) >> dsl::p<nested_expression> + LEXY_LIT(":");
//
// 				using operand = comparison;
// 			};
//
// 			struct assignment : public dsl::infix_op_single
// 			{
// 				// Similar to * above, we need to prevent `=` from matching `==`.
// 				constexpr static auto op = dsl::op<void>(dsl::not_followed_by(LEXY_LIT("="), LEXY_LIT("=")));
//
// 				using operand = condition;
// 			};
//
// 			// An expression also needs to specify the operation with the lowest binding power.
// 			// The operation of everything else is determined by following the `::operand` member.
// 			using operation = assignment;
//
// 			constexpr static auto value =
// 					// We need a sink as the comparison expression generates a list.
// 					lexy::fold_inplace<parse::node_expression_ptr<parse::node_expression_composite>>(
// 							[] { return parse::make_expression<parse::node_expression_composite>(); },
// 							[](parse::node_expression_composite& node, parse::node_expression_ptr<> expression) { node.push_operand(std::move(expression)); })
// 					// The result of the list feeds into a callback that handles all other cases.
// 					>> lexy::callback(
// 							// atoms
// 							lexy::forward<parse::node_expression_ptr<>>,
// 							lexy::new_<parse::node_expression_literal, parse::node_expression_ptr<>>,
// 							lexy::new_<parse::node_expression_identifier, parse::node_expression_ptr<>>,
// 							// unary/binary operators
// 							lexy::new_<parse::node_expression_binary_arithmetic, parse::node_expression_ptr<>>,
// 							lexy::new_<parse::node_expression_unary_arithmetic, parse::node_expression_ptr<>>,
// 							// conditional and assignment
// 							lexy::new_<parse::node_expression_assignment, parse::node_expression_ptr<>>);
// 		};
//
// 		struct node : public lexy::token_production
// 		{
// 			struct expected_node
// 			{
// 				consteval static auto name() noexcept { return "expected node here"; }
// 			};
//
// 			constexpr static auto rule = []
// 			{
// 				constexpr auto primitive =
// 						dsl::p<boolean> |
// 						dsl::p<number> |
// 						dsl::p<string>;
// 				constexpr auto complex =
// 						dsl::p<expression>;
//
// 				return dsl::p<comment> | primitive | /* complex | */ dsl::error<expected_node>;
// 			}();
//
// 			constexpr static auto value = lexy::construct<parse::Node>;
// 		};
//
// 		// ===================================
// 		// =========== PRODUCTION ============
// 		// ===================================
// 		struct production
// 		{
// 			constexpr static auto name = "production";
//
// 			// todo
// 			constexpr static auto max_recursion_depth = 32;
//
// 			// Whitespace is a sequence of space, tab, carriage return, or newline.
// 			constexpr static auto whitespace = dsl::ascii::space;
//
// 			struct node_tree_part
// 			{
// 				constexpr static auto rule = dsl::brackets(
// 						LEXY_LIT("BEGIN"),
// 						LEXY_LIT("END")
// 						).opt_list(
// 						dsl::recurse<node>//,
// 						// dsl::sep(dsl::peek(dsl::eol))
// 						);
//
// 				constexpr static auto value = lexy::as_list<parse::node_tree_result>;
// 			};
//
// 			constexpr static auto rule = dsl::p<node_tree_part>;
//
// 			constexpr static auto value = lexy::construct<parse::node_tree_result>;
// 		};
// 	}
// }
//
// namespace gal::gsl::parse
// {
// 	auto parse_file(const string::string_view filename) -> node_tree_result
// 	{
// 		const auto file = lexy::read_file<lexy::utf8_encoding>(filename.data());
// 		if (!file)
// 		{
// 			// todo
// 			throw std::exception{"Cannot read file!"};
// 		}
//
// 		auto production = lexy::parse<parser::production>(file.buffer(), lexy_ext::report_error);
// 		if (!production.has_value())
// 		{
// 			// todo
// 			throw std::exception{"Context error!"};
// 		}
//
// 		return std::move(production).value();
// 	}
// }
