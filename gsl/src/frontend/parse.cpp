#include <gsl/frontend/parse.hpp>
#include <gsl/backend/ast.hpp>

#include <lexy/dsl.hpp>
#include <lexy/action/parse.hpp>
#include <lexy/action/trace.hpp>
#include <lexy/input/file.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy_ext/report_error.hpp>
#include <lexy/callback.hpp>
#include <lexy/visualize.hpp>

#define GSL_PARSE_ERROR_OUTPUT stderr
#define GSL_TRACE_PARSE
#define GSL_TRACE_PARSE_OUTPUT stdout

#include <optional>
#include <cstdio>
#include <stdexcept>

namespace
{
	namespace gsl = gal::gsl;

	using symbol_name = gsl::ast::symbol_name;
	using symbol_name_view = gsl::ast::symbol_name_view;

	class ParseState
	{
	public:
		template<typename Return, typename... Functions>
		[[nodiscard]] constexpr static auto callback(Functions&&... functions)
		{
			return lexy::bind(
					lexy::callback<Return>(std::forward<Functions>(functions)...),
					// out parse state
					lexy::parse_state,
					// parsed values
					lexy::values);
		}

		using context_type = lexy::buffer<lexy::utf8_encoding>;
		using char_type = context_type::char_type;

		gsl::string::string filename;
		context_type buffer;
		lexy::input_location_anchor<context_type> buffer_anchor;

		gsl::ast::module_type mod;

		gsl::ast::structure_type current_structure;
		gsl::ast::function_type current_function;

		ParseState(gsl::string::string&& filename, context_type&& buffer)
			: filename{std::move(filename)},
			  buffer{std::move(buffer)},
			  buffer_anchor{this->buffer} {}

		auto report_invalid_identifier(const char_type* position, const symbol_name_view identifier, const char* category) const -> void
		{
			const auto location = lexy::get_input_location(buffer, position, buffer_anchor);

			const auto out = lexy::cfile_output_iterator{GSL_PARSE_ERROR_OUTPUT};
			const lexy_ext::diagnostic_writer writer{buffer, {.flags = lexy::visualize_fancy}};

			(void)writer.write_message(out,
			                           lexy_ext::diagnostic_kind::error,
			                           [&](lexy::cfile_output_iterator, lexy::visualization_options)
			                           {
				                           (void)std::fprintf(GSL_PARSE_ERROR_OUTPUT, "unknown %s name '%s'", category, identifier.data());
				                           return out;
			                           });

			if (!filename.empty()) { (void)writer.write_path(out, filename.c_str()); }

			(void)writer.write_empty_annotation(out);
			(void)writer.write_annotation(
					out,
					lexy_ext::annotation_kind::primary,
					location,
					identifier.size(),
					[&](lexy::cfile_output_iterator, lexy::visualization_options)
					{
						(void)std::fprintf(GSL_PARSE_ERROR_OUTPUT, "used here");
						return out;
					});
		}

		auto report_invalid_expression(const char_type* position, const symbol_name_view identifier, const char* category) const -> void
		{
			const auto location = lexy::get_input_location(buffer, position, buffer_anchor);

			const auto out = lexy::cfile_output_iterator{GSL_PARSE_ERROR_OUTPUT};
			const lexy_ext::diagnostic_writer writer{buffer, {.flags = lexy::visualize_fancy}};

			(void)writer.write_message(out,
			                           lexy_ext::diagnostic_kind::error,
			                           [&](lexy::cfile_output_iterator, lexy::visualization_options)
			                           {
				                           (void)std::fprintf(GSL_PARSE_ERROR_OUTPUT, "invalid %s expression passed", category);
				                           return out;
			                           });

			if (!filename.empty()) { (void)writer.write_path(out, filename.c_str()); }

			(void)writer.write_empty_annotation(out);
			(void)writer.write_annotation(
					out,
					lexy_ext::annotation_kind::primary,
					location,
					identifier.size(),
					[&](lexy::cfile_output_iterator, lexy::visualization_options)
					{
						(void)std::fprintf(GSL_PARSE_ERROR_OUTPUT, "for '%s'", identifier.data());
						return out;
					});
		}

		auto report_duplicate_declaration(const char_type* position, const symbol_name_view identifier, const char* category) const -> void
		{
			const auto location = lexy::get_input_location(buffer, position, buffer_anchor);

			const auto out = lexy::cfile_output_iterator{GSL_PARSE_ERROR_OUTPUT};
			const lexy_ext::diagnostic_writer writer{buffer, {.flags = lexy::visualize_fancy}};

			(void)writer.write_message(out,
			                           lexy_ext::diagnostic_kind::error,
			                           [&](lexy::cfile_output_iterator, lexy::visualization_options)
			                           {
				                           (void)std::fprintf(GSL_PARSE_ERROR_OUTPUT, "duplicate %s declaration named '%s'", category, identifier.data());
				                           return out;
			                           });

			if (!filename.empty()) { (void)writer.write_path(out, filename.c_str()); }

			(void)writer.write_empty_annotation(out);
			(void)writer.write_annotation(
					out,
					lexy_ext::annotation_kind::primary,
					location,
					identifier.size(),
					[&](lexy::cfile_output_iterator, lexy::visualization_options)
					{
						(void)std::fprintf(GSL_PARSE_ERROR_OUTPUT, "second declaration here");
						return out;
					});
		}

		auto report_shadow_declaration(const char_type* position, const symbol_name_view identifier, const char* category) const -> void
		{
			const auto location = lexy::get_input_location(buffer, position, buffer_anchor);

			const auto out = lexy::cfile_output_iterator{GSL_PARSE_ERROR_OUTPUT};
			const lexy_ext::diagnostic_writer writer{buffer, {.flags = lexy::visualize_fancy}};

			(void)writer.write_message(out,
			                           lexy_ext::diagnostic_kind::warning,
			                           [&](lexy::cfile_output_iterator, lexy::visualization_options)
			                           {
				                           (void)std::fprintf(GSL_PARSE_ERROR_OUTPUT, "shadow %s declaration named '%s'", category, identifier.data());
				                           return out;
			                           });

			if (!filename.empty()) { (void)writer.write_path(out, filename.c_str()); }

			(void)writer.write_empty_annotation(out);
			(void)writer.write_annotation(
					out,
					lexy_ext::annotation_kind::primary,
					location,
					identifier.size(),
					[&](lexy::cfile_output_iterator, lexy::visualization_options)
					{
						(void)std::fprintf(GSL_PARSE_ERROR_OUTPUT, "second declaration here");
						return out;
					});
		}
	};
}

namespace grammar
{
	namespace dsl = lexy::dsl;

	struct identifier
	{
		constexpr static auto rule =
				dsl::identifier(
						// begin with alpha/underscore
						dsl::ascii::alpha_underscore,
						// continue with alpha/digit/underscore
						dsl::ascii::alpha_digit_underscore);

		constexpr static auto value = lexy::as_string<symbol_name>;
	};

	// type[1][2][3][4]...
	struct type_declaration
	{
		struct dimension_list
		{
			using dimension_type = gsl::ast::TypeDeclaration::dimension_type;
			using dimension_container_type = gsl::ast::TypeDeclaration::dimension_container_type;

			constexpr static auto rule =
					LEXY_DEBUG("parse type dimension begin") +
					dsl::list(dsl::lit_c<'['>//
					          >> (LEXY_DEBUG("parse a dimension number") +
					              dsl::integer<dimension_type> + dsl::lit_c<']'>
					          )) +
					LEXY_DEBUG("parse type dimension end");

			constexpr static auto value =
					lexy::fold_inplace<dimension_container_type>(
							[] { return dimension_container_type{}; },
							[](dimension_container_type& dimensions, const dimension_type i) -> void { dimensions.push_back(i); });
		};

		constexpr static auto rule =
				LEXY_DEBUG("parse type declaration begin") +
				dsl::position +
				// type
				LEXY_DEBUG("parse type name") +
				dsl::p<identifier> +
				// dimensions
				dsl::opt(
						dsl::peek(dsl::lit_c<'['>)
						>> dsl::p<dimension_list>) +
				LEXY_DEBUG("parse type declaration end");

		constexpr static auto value = ParseState::callback<gsl::ast::type_declaration_type>(
				// with dimensions
				[](const ParseState& state, const ParseState::char_type* type_position, symbol_name&& type_name, gsl::ast::TypeDeclaration::dimension_container_type&& dimensions) -> gsl::ast::type_declaration_type
				{
					// parse type
					gsl::ast::structure_type target_structure;
					auto type = gsl::ast::TypeDeclaration::parse_type(type_name);

					if (type == gsl::ast::TypeDeclaration::variable_type::NIL)
					{
						// maybe structure?
						if (target_structure = state.mod->get_structure(type_name);
							!target_structure) { state.report_invalid_identifier(type_position, type_name, "type"); }
					}
					else
					{
						// builtin
					}

					return gsl::ast::make<gsl::ast::TypeDeclaration>(
							type,
							target_structure.get(),
							std::move(dimensions)
							);
				},
				// without dimensions
				[](const ParseState& state, const ParseState::char_type* type_position, symbol_name&& type_name, lexy::nullopt) -> gsl::ast::type_declaration_type
				{
					// parse type
					gsl::ast::structure_type target_structure;
					auto type = gsl::ast::TypeDeclaration::parse_type(type_name);

					if (type == gsl::ast::TypeDeclaration::variable_type::NIL)
					{
						// maybe structure?
						if (target_structure = state.mod->get_structure(type_name);
							!target_structure) { state.report_invalid_identifier(type_position, type_name, "type"); }
						type = gsl::ast::TypeDeclaration::variable_type::STRUCTURE;
					}
					else
					{
						// builtin
					}

					return gsl::ast::make<gsl::ast::TypeDeclaration>(
							type,
							target_structure.get()
							);
				});
	};

	// type_declaration variable
	struct variable_declaration
	{
		constexpr static auto whitespace = dsl::ascii::blank;

		constexpr static auto rule =
				LEXY_DEBUG("parse variable declaration begin") +
				dsl::position +
				dsl::p<type_declaration> +
				LEXY_DEBUG("parse variable name") +
				dsl::p<identifier> +
				LEXY_DEBUG("parse type declaration end");

		constexpr static auto value = ParseState::callback<gsl::ast::Variable::variable_declaration>(
				[](const ParseState& state, const ParseState::char_type* type_position, gsl::ast::type_declaration_type&& type_declaration, symbol_name&& type_name) -> gsl::ast::Variable::variable_declaration
				{
					if (type_declaration->type() == gsl::ast::TypeDeclaration::variable_type::VOID)
					{
						// todo: report 'void' is not a valid type for variable?
						//
						state.report_invalid_identifier(type_position, type_name, "type");
					}

					return gsl::ast::Variable::variable_declaration{.name = std::move(type_name), .type = std::move(type_declaration)};
				}
				);
	};

	struct expression : public lexy::expression_production
	{
		struct expected_operand
		{
			static constexpr auto name = "expected operand";
		};

		// An expression that is nested inside another expression.
		struct nested_expression : public lexy::transparent_production
		{
			// We change the whitespace rule to allow newlines:
			// as it's nested, the REPL can properly handle continuation lines.
			constexpr static auto whitespace = dsl::ascii::space | dsl::backslash >> dsl::newline;

			constexpr static auto rule = LEXY_DEBUG("parse nested expression begin") + dsl::recurse<expression> + LEXY_DEBUG("parse nested expression end");

			constexpr static auto value = lexy::forward<gsl::ast::expression_type>;
		};

		struct noop
		{
			constexpr static auto rule = LEXY_LIT("pass");

			constexpr static auto value = lexy::callback<gsl::ast::expression_type>(
					[]() -> gsl::ast::expression_type { return gsl::ast::make<gsl::ast::ExpressionNoop>(); });
		};

		// ================================
		// =============literal===============
		// ================================

		struct literal_bool : public lexy::token_production
		{
			struct literal_true : public lexy::transparent_production
			{
				constexpr static auto rule = LEXY_LIT("true");
				constexpr static auto value = lexy::constant(true);
			};

			struct literal_false : public lexy::transparent_production
			{
				constexpr static auto rule = LEXY_LIT("false");
				constexpr static auto value = lexy::constant(false);
			};

			constexpr static auto rule = dsl::p<literal_true> | dsl::p<literal_false>;
			constexpr static auto value = lexy::callback<gsl::ast::expression_type>(
					[](const bool value) -> gsl::ast::expression_type { return gsl::ast::make<gsl::ast::ExpressionConstantBoolean>(value); }
					);
		};

		struct literal_int : public lexy::token_production
		{
			using value_type = gsl::ast::ExpressionConstantInt::value_type;

			constexpr static auto rule = LEXY_LIT("0x") >> dsl::integer<value_type, dsl::hex> | dsl::integer<value_type>;
			constexpr static auto value =
					lexy::callback<gsl::ast::expression_type>(
							[](const value_type value) -> gsl::ast::expression_type { return gsl::ast::make<gsl::ast::ExpressionConstantInt>(value); });
		};

		struct literal_float : public lexy::token_production
		{
			using value_type = gsl::ast::ExpressionConstantFloat::value_type;
			using fractional_type = gsl::ast::ExpressionConstantFloat::fractional_type::value_type;
			using exponent_type = gsl::ast::ExpressionConstantFloat::exponent_type::value_type;

			struct value_part : public lexy::transparent_production
			{
				constexpr static auto rule = dsl::minus_sign + dsl::integer<value_type>(dsl::digits<>.no_leading_zero());
				constexpr static auto value = lexy::as_integer<value_type>;
			};

			struct fractional_part : public lexy::transparent_production
			{
				constexpr static auto rule = dsl::lit_c<'.'>//
				                             >> dsl::integer<value_type>(dsl::digits<>.no_leading_zero());
				constexpr static auto value = lexy::as_integer<fractional_type>;
			};

			struct exponent_part : public lexy::transparent_production
			{
				constexpr static auto rule = (dsl::lit_c<'e'> | dsl::lit_c<'E'>) >> (dsl::sign + dsl::integer<exponent_type>);
				constexpr static auto value = lexy::as_integer<exponent_type>;
			};

			constexpr static auto rule = dsl::peek(dsl::lit_c<'-'> / dsl::digit<>)//
			                             >> (dsl::p<value_part> + dsl::opt(dsl::p<fractional_part>) + dsl::opt(dsl::p<exponent_part>));
			constexpr static auto value = lexy::callback<gsl::ast::expression_type>(
					[](
					const gsl::ast::ExpressionConstantFloat::value_type& value,
					const gsl::ast::ExpressionConstantFloat::fractional_type& fractional,
					const gsl::ast::ExpressionConstantFloat::exponent_type& exponent) -> gsl::ast::expression_type
					{
						return gsl::ast::make<gsl::ast::ExpressionConstantFloat>(value, fractional, exponent);
					});
		};

		struct literal_string : public lexy::token_production
		{
			using value_type = gsl::ast::ExpressionConstantString::value_type;

			struct value_part : public lexy::transparent_production
			{
				constexpr static auto rule = dsl::quoted(
						// all printable unicode characters in single quotation marks
						dsl::unicode::print);

				constexpr static auto value = lexy::as_string<value_type>;
			};

			constexpr static auto rule = dsl::p<value_part>;

			constexpr static auto value = lexy::callback<gsl::ast::expression_type>(
					[](value_type&& value) -> gsl::ast::expression_type { return gsl::ast::make<gsl::ast::ExpressionConstantString>(std::move(value)); });
		};

		// We need to specify the atomic part of an expression.
		// todo
		constexpr static auto atom = []
		{
			constexpr auto paren_expression = dsl::parenthesized(dsl::p<nested_expression>);
			// todo: function call?

			constexpr auto literals = dsl::p<literal_bool> | dsl::p<literal_int> | dsl::p<literal_float> | dsl::p<literal_string>;

			return
					LEXY_DEBUG("parse expression begin") +
					dsl::position +
					(paren_expression |
					 literals |
					 dsl::p<noop> |
					 dsl::error<expected_operand>) + LEXY_DEBUG("parse expression end");
		}();

		// ================================
		// ==========unary operator============
		// ================================

		// ~x
		struct unary_bit_complement : public dsl::prefix_op
		{
			constexpr static auto op = dsl::op<gsl::ast::ExpressionUnaryOperator::operand_type::BIT_COMPLEMENT>(LEXY_LIT("~"));
			using operand = dsl::atom;
		};

		// -x
		struct unary_negate : public dsl::prefix_op
		{
			constexpr static auto op = dsl::op<gsl::ast::ExpressionUnaryOperator::operand_type::NEGATE>(LEXY_LIT("-"));
			using operand = dsl::atom;
		};

		// !x
		struct unary_logical_not : public dsl::prefix_op
		{
			constexpr static auto op = dsl::op<gsl::ast::ExpressionUnaryOperator::operand_type::LOGICAL_NOT>(LEXY_LIT("!"));
			using operand = dsl::atom;
		};

		// ================================
		// ==========binary operator============
		// ================================

		// x ** x
		struct binary_power : public dsl::infix_op_left// todo: left or right?
		{
			constexpr static auto op = dsl::op<gsl::ast::ExpressionBinaryOperator::operand_type::POW>(LEXY_LIT("**"));
			using operand = dsl::atom;
		};

		// x * x
		struct binary_multiply : public dsl::infix_op_left
		{
			// Since we have both ** and * as possible operators, we need to ensure that * is only
			// matched when it is not followed by *.
			// In the particular situation where ** has higher binding power, it doesn't actually
			// matter here, but it's better to be more resilient.
			constexpr static auto op = dsl::op<gsl::ast::ExpressionBinaryOperator::operand_type::MULTIPLY>(dsl::not_followed_by(LEXY_LIT("*"), LEXY_LIT("*")));
			using operand = binary_power;
		};

		// x / x
		struct binary_divide : public dsl::infix_op_left
		{
			constexpr static auto op = dsl::op<gsl::ast::ExpressionBinaryOperator::operand_type::DIVIDE>(LEXY_LIT("/"));
			using operand = binary_multiply;
		};

		// x + x
		struct binary_plus : public dsl::infix_op_left
		{
			constexpr static auto op = dsl::op<gsl::ast::ExpressionBinaryOperator::operand_type::PLUS>(LEXY_LIT("+"));
			using operand = binary_divide;
		};

		// x - x
		struct binary_minus : public dsl::infix_op_left
		{
			constexpr static auto op = dsl::op<gsl::ast::ExpressionBinaryOperator::operand_type::MINUS>(LEXY_LIT("-"));
			using operand = binary_plus;
		};

		// x & x
		struct binary_bit_and : public dsl::infix_op_left
		{
			constexpr static auto op = dsl::op<gsl::ast::ExpressionBinaryOperator::operand_type::BIT_AND>(LEXY_LIT("&"));
			using operand = unary_bit_complement;
		};

		// x | x
		struct binary_bit_or : public dsl::infix_op_left
		{
			constexpr static auto op = dsl::op<gsl::ast::ExpressionBinaryOperator::operand_type::BIT_OR>(LEXY_LIT("|"));
			using operand = binary_bit_and;
		};

		// x ^ x
		struct binary_bit_xor : public dsl::infix_op_left
		{
			constexpr static auto op = dsl::op<gsl::ast::ExpressionBinaryOperator::operand_type::BIT_XOR>(LEXY_LIT("^"));
			using operand = binary_bit_and;
		};

		// x == y / x >= y / x > y / x <= y / x < y
		// struct binary_comparison : public dsl::infix_op_list
		// {
		// private:
		// 	using enum gsl::ast::ExpressionBinaryOperator::operand_type;
		// public:
		// 	constexpr static auto op =
		// 			dsl::op<EQUAL>(LEXY_LIT("==")) /
		// 			dsl::op<GREATER_EQUAL>(LEXY_LIT(">=")) /
		// 			dsl::op<GREATER>(dsl::not_followed_by(LEXY_LIT(">"), LEXY_LIT("="))) /
		// 			dsl::op<LESS_EQUAL>(LEXY_LIT("<=")) /
		// 			dsl::op<LESS>(dsl::not_followed_by(LEXY_LIT("<"), LEXY_LIT("=")));
		//
		// 	// The use of dsl::groups ensures that an expression can either contain
		// 	// mathematical or bit operators.
		// 	// Mixing requires parenthesis.
		// 	using operand = dsl::groups<
		// 		// the lowest priority mathematical operation
		// 		binary_minus,
		// 		// the lowest priority bit operation
		// 		binary_bit_xor>;
		// };

		// An expression also needs to specify the operation with the lowest binding power.
		// The operation of everything else is determined by following the `::operand` member.
		// using operation = binary_comparison;
		using operation = binary_bit_xor;

		// todo
		constexpr static auto value = ParseState::callback<gsl::ast::expression_type>(
				// atom
				[](const ParseState& state, const ParseState::char_type* position, gsl::ast::expression_type&& expression) -> gsl::ast::expression_type
				{
					// todo
					if (!expression) { state.report_invalid_expression(position, "expression", "expression"); }

					return expression;
				},
				// ================================
				// ==========unary operator============
				// ================================
				[]([[maybe_unused]] const ParseState& state, lexy::op<unary_bit_complement::op>, gsl::ast::expression_type&& expression) -> gsl::ast::expression_type { return gsl::ast::make<gsl::ast::ExpressionUnaryOperator>(gsl::ast::ExpressionUnaryOperator::operand_type::BIT_COMPLEMENT, std::move(expression)); },
				[]([[maybe_unused]] const ParseState& state, lexy::op<unary_negate::op>, gsl::ast::expression_type&& expression) -> gsl::ast::expression_type { return gsl::ast::make<gsl::ast::ExpressionUnaryOperator>(gsl::ast::ExpressionUnaryOperator::operand_type::NEGATE, std::move(expression)); },
				// ================================
				// ==========binary operator============
				// ================================
				[]([[maybe_unused]] const ParseState& state, gsl::ast::expression_type&& lhs, lexy::op<binary_power::op>, gsl::ast::expression_type&& rhs) -> gsl::ast::expression_type
				{
					return gsl::ast::make<gsl::ast::ExpressionBinaryOperator>(
							gsl::ast::ExpressionBinaryOperator::operand_type::POW,
							std::move(lhs),
							std::move(rhs));
				},
				[]([[maybe_unused]] const ParseState& state, gsl::ast::expression_type&& lhs, lexy::op<binary_multiply::op>, gsl::ast::expression_type&& rhs) -> gsl::ast::expression_type
				{
					return gsl::ast::make<gsl::ast::ExpressionBinaryOperator>(
							gsl::ast::ExpressionBinaryOperator::operand_type::MULTIPLY,
							std::move(lhs),
							std::move(rhs));
				},
				[]([[maybe_unused]] const ParseState& state, gsl::ast::expression_type&& lhs, lexy::op<binary_divide::op>, gsl::ast::expression_type&& rhs) -> gsl::ast::expression_type
				{
					return gsl::ast::make<gsl::ast::ExpressionBinaryOperator>(
							gsl::ast::ExpressionBinaryOperator::operand_type::DIVIDE,
							std::move(lhs),
							std::move(rhs));
				},
				[]([[maybe_unused]] const ParseState& state, gsl::ast::expression_type&& lhs, lexy::op<binary_plus::op>, gsl::ast::expression_type&& rhs) -> gsl::ast::expression_type
				{
					return gsl::ast::make<gsl::ast::ExpressionBinaryOperator>(
							gsl::ast::ExpressionBinaryOperator::operand_type::PLUS,
							std::move(lhs),
							std::move(rhs));
				},
				[]([[maybe_unused]] const ParseState& state, gsl::ast::expression_type&& lhs, lexy::op<binary_minus::op>, gsl::ast::expression_type&& rhs) -> gsl::ast::expression_type
				{
					return gsl::ast::make<gsl::ast::ExpressionBinaryOperator>(
							gsl::ast::ExpressionBinaryOperator::operand_type::MINUS,
							std::move(lhs),
							std::move(rhs));
				},
				[]([[maybe_unused]] const ParseState& state, gsl::ast::expression_type&& lhs, lexy::op<binary_bit_and::op>, gsl::ast::expression_type&& rhs) -> gsl::ast::expression_type
				{
					return gsl::ast::make<gsl::ast::ExpressionBinaryOperator>(
							gsl::ast::ExpressionBinaryOperator::operand_type::BIT_AND,
							std::move(lhs),
							std::move(rhs));
				},
				[]([[maybe_unused]] const ParseState& state, gsl::ast::expression_type&& lhs, lexy::op<binary_bit_or::op>, gsl::ast::expression_type&& rhs) -> gsl::ast::expression_type
				{
					return gsl::ast::make<gsl::ast::ExpressionBinaryOperator>(
							gsl::ast::ExpressionBinaryOperator::operand_type::BIT_OR,
							std::move(lhs),
							std::move(rhs));
				},
				[]([[maybe_unused]] const ParseState& state, gsl::ast::expression_type&& lhs, lexy::op<binary_bit_xor::op>, gsl::ast::expression_type&& rhs) -> gsl::ast::expression_type
				{
					return gsl::ast::make<gsl::ast::ExpressionBinaryOperator>(
							gsl::ast::ExpressionBinaryOperator::operand_type::BIT_XOR,
							std::move(lhs),
							std::move(rhs));
				}
				);
	};

	struct variable_declaration_with_assignment
	{
		constexpr static auto rule =
				LEXY_DEBUG("parse variable with assignment begin") +
				dsl::p<variable_declaration> + dsl::equal_sign + dsl::position + dsl::p<expression> +
				LEXY_DEBUG("parse variable with assignment end");

		constexpr static auto value = ParseState::callback<gsl::ast::variable_type>(
				[](const ParseState& state, gsl::ast::Variable::variable_declaration&& variable_declaration, const ParseState::char_type* position, gsl::ast::expression_type&& expression) -> gsl::ast::variable_type
				{
					if (!expression) { state.report_invalid_expression(position, variable_declaration.name, "variable"); }

					return gsl::ast::make<gsl::ast::Variable>(
							std::move(variable_declaration),
							std::move(expression)
							);
				});
	};

	struct variable_declaration_with_optional_assign
	{
		constexpr static auto rule =
				LEXY_DEBUG("parse variable with optional assignment begin") +
				dsl::p<variable_declaration> + dsl::opt(dsl::equal_sign >> dsl::position + dsl::p<expression>) +
				LEXY_DEBUG("parse variable with optional assignment end");

		constexpr static auto value = ParseState::callback<gsl::ast::variable_type>(
				// with assignment
				[](const ParseState& state, gsl::ast::Variable::variable_declaration&& variable_declaration, const ParseState::char_type* position, gsl::ast::expression_type&& expression) -> gsl::ast::variable_type
				{
					if (!expression) { state.report_invalid_expression(position, variable_declaration.name, "variable"); }

					return gsl::ast::make<gsl::ast::Variable>(
							std::move(variable_declaration),
							std::move(expression));
				},
				// without assignment
				[](const ParseState& state, gsl::ast::Variable::variable_declaration&& variable_declaration, lexy::nullopt) -> gsl::ast::variable_type
				{
					(void)state;
					return gsl::ast::make<gsl::ast::Variable>(std::move(variable_declaration));
				});
	};

	struct structure_declaration
	{
		[[nodiscard]] consteval static auto name() noexcept { return "structure declaration"; }

		// struct name
		struct header
		{
			constexpr static auto rule = LEXY_DEBUG("parse structure name") + dsl::position + dsl::p<identifier>;

			constexpr static auto value = ParseState::callback<void>(
					[](ParseState& state, const ParseState::char_type* position, symbol_name&& symbol) -> void
					{
						auto [success, structure] = state.mod->register_structure(std::move(symbol));
						if (!success) { state.report_duplicate_declaration(position, structure->get_name(), "structure"); }

						state.current_structure = structure;
					});
		};

		struct field
		{
			// todo: allow assignment?
			constexpr static auto rule = LEXY_DEBUG("parse structure field") + dsl::position + dsl::p<variable_declaration>;

			constexpr static auto value = ParseState::callback<void>(
					[](const ParseState& state, const ParseState::char_type* position, gsl::ast::Variable::variable_declaration&& variable) -> void
					{
						if (!state.current_structure->register_field(std::move(variable)))
						{
							// We know that if the insertion fails, the 'symbol' will not be moved, so please shut up
							state.report_duplicate_declaration(position, variable.name, "field");// NOLINT(bugprone-use-after-move)
						}
					});
		};

		constexpr static auto rule =
				LEXY_KEYWORD("struct", identifier::rule) >>
				(LEXY_DEBUG("parse structure declaration begin") +
				 dsl::p<header> +
				 // todo: forward declaration?
				 dsl::curly_bracketed.opt_list(dsl::p<field>, dsl::trailing_sep(dsl::newline | dsl::semicolon)) +
				 LEXY_DEBUG("parse structure declaration end"));

		constexpr static auto value = lexy::forward<void>;
	};

	struct global_declaration
	{
		[[nodiscard]] consteval static auto name() noexcept { return "global variable declaration"; }

		struct mutable_global
		{
			[[nodiscard]] consteval static auto name() noexcept { return "global mutable variable declaration"; }

			constexpr static auto rule = []
			{
				// mut [type variable] = expression
				// immutable variable should have initializer
				return LEXY_KEYWORD("mut", identifier::rule) >> (dsl::position + dsl::p<variable_declaration_with_optional_assign>);
			}();

			constexpr static auto value = ParseState::callback<void>(
					[](const ParseState& state, const ParseState::char_type* position, gsl::ast::variable_type&& variable) -> void
					{
						// try register
						auto [success, v] = state.mod->register_global_immutable(variable->get_name());
						if (!success) { state.report_duplicate_declaration(position, v->get_name(), "global"); }

						// register succeeded, reset the pointer
						// Note: if the global variables are duplicate defined, the original global variables will be overwritten
						// todo: better swap?
						*v = std::move(*variable);
					});
		};

		struct immutable_global
		{
			[[nodiscard]] consteval static auto name() noexcept { return "global immutable variable declaration"; }

			static constexpr auto rule = []
			{
				// [type variable] [= expression]
				// mutable variable may not have initializer
				return dsl::else_ >> (dsl::position + dsl::p<variable_declaration_with_assignment>);
			}();

			static constexpr auto value = ParseState::callback<void>(
					[](const ParseState& state, const ParseState::char_type* position, gsl::ast::variable_type&& variable) -> void
					{
						// try register
						auto [success, v] = state.mod->register_global_immutable(variable->get_name());
						if (!success) { state.report_duplicate_declaration(position, v->get_name(), "global"); }

						// register succeeded, reset the pointer
						// Note: if the global variables are duplicate defined, the original global variables will be overwritten
						// todo: better swap?
						*v = std::move(*variable);
					}
					);
		};

		constexpr static auto rule =
				LEXY_KEYWORD("global", identifier::rule) >>
				(
					LEXY_DEBUG("parse global declaration begin") +
					(dsl::p<mutable_global> | dsl::p<immutable_global>) +
					// semicolon required ?
					dsl::semicolon +
					LEXY_DEBUG("parse global declaration end"));

		constexpr static auto value = lexy::forward<void>;
	};

	struct function_declaration
	{
		[[nodiscard]] consteval static auto name() noexcept { return "function declaration"; }

		struct header
		{
			struct arguments_list
			{
				struct argument
				{
					constexpr static auto rule = dsl::position + dsl::p<variable_declaration_with_optional_assign>;

					constexpr static auto value = ParseState::callback<gsl::ast::variable_type>(
							[](const ParseState& state, const ParseState::char_type* position, gsl::ast::variable_type&& variable) -> gsl::ast::variable_type
							{
								// shadow check
								if (state.mod->has_global(variable->get_name())) { state.report_shadow_declaration(position, variable->get_name(), "global"); }

								// return
								return std::move(variable);
							});
				};

				constexpr static auto rule = dsl::round_bracketed.opt_list(
						dsl::p<argument>,
						dsl::sep(dsl::comma));

				constexpr static auto value = lexy::as_list<gsl::ast::Function::arguments_container_type>;
			};

			struct return_type_expected
			{
				consteval static auto name() { return "return type expected."; }
			};

			constexpr static auto rule =
					// function name
					dsl::position +
					LEXY_DEBUG("parse function name") +
					dsl::p<identifier> +
					// arguments
					LEXY_DEBUG("parse function arguments") +
					dsl::p<arguments_list> +
					// return type
					// todo: optional return type?
					LEXY_LIT("->").error<return_type_expected> +
					LEXY_DEBUG("parse function return type") +
					dsl::p<type_declaration>;

			constexpr static auto value = ParseState::callback<void>(
					[](
					ParseState& state,
					// function name
					const ParseState::char_type* function_position,
					symbol_name&& function_name,
					// arguments
					gsl::ast::Function::arguments_container_type&& arguments,
					// return type
					gsl::ast::type_declaration_type&& return_type)
					{
						auto [success, function] = state.mod->register_function(std::move(function_name));
						if (!success) { state.report_duplicate_declaration(function_position, function->get_name(), "function"); }

						// set arguments
						function->set_arguments(std::move(arguments));
						// set return type
						function->set_return_type(std::move(return_type));

						state.current_function = function;
					});
		};

		struct body
		{
			constexpr static auto rule = dsl::curly_bracketed.open() >> (
				                             LEXY_DEBUG("parse function body begin") +
				                             dsl::position + dsl::p<expression> + dsl::curly_bracketed.close() +
				                             LEXY_DEBUG("parse function body end")
			                             );

			constexpr static auto value = ParseState::callback<void>(
					[](const ParseState& state, const ParseState::char_type* position, gsl::ast::expression_type&& body) -> void
					{
						if (!body) { state.report_invalid_expression(position, state.current_function->get_name(), "function body"); }

						state.current_function->set_function_body(std::move(body));
					}
					);
		};

		constexpr static auto rule =
				LEXY_KEYWORD("fn", identifier::rule) >>
				// function declaration
				(
					LEXY_DEBUG("parse function declaration begin") +
					dsl::p<header> +
					// function body
					dsl::p<body> +
					LEXY_DEBUG("parse function declaration end"));

		constexpr static auto value = lexy::forward<void>;
	};

	// module module_name;
	struct module_declaration
	{
		[[nodiscard]] consteval static auto name() noexcept { return "module declaration"; }

		constexpr static auto whitespace =
				// space
				dsl::ascii::blank |
				// new line
				dsl::newline |
				// comment
				dsl::hash_sign >> dsl::until(dsl::newline);

		struct header
		{
			constexpr static auto rule =
					LEXY_KEYWORD("module", identifier::rule) +
					// todo: check module duplicate?
					LEXY_DEBUG("parse module name") +
					dsl::p<identifier> +
					// semicolon required ?
					dsl::semicolon;

			constexpr static auto value = ParseState::callback<void>(
					[](ParseState& state, symbol_name&& symbol) -> void { state.mod = gsl::ast::make<gsl::ast::Module>(std::move(symbol)); });
		};

		constexpr static auto rule =
				LEXY_DEBUG("module declaration begin") +
				dsl::p<header> +
				dsl::terminator(dsl::eof).opt_list(
						dsl::p<global_declaration> |
						dsl::p<function_declaration> |
						dsl::p<structure_declaration>
						) +
				LEXY_DEBUG("module declaration end");

		constexpr static auto value = lexy::forward<void>;
	};
}

namespace gal::gsl::frontend
{
	auto parse_file(const string::string_view filename) -> ast::module_type
	{
		auto file = lexy::read_file<lexy::utf8_encoding>(filename.data());

		if (!file)
		{
			// todo
			throw std::runtime_error{"Cannot read file!"};
		}

		ParseState state{string::string{filename}, std::move(file).buffer()};

		if (const auto result =
					lexy::parse<grammar::module_declaration>(state.buffer, state, lexy_ext::report_error.opts({.flags = lexy::visualize_fancy}).path(state.filename.c_str()));
			!result.is_success())
		{
			// todo: handle it?
			throw std::runtime_error{"Cannot parse file!"};
		}

		#ifdef GSL_TRACE_PARSE
		lexy::trace<grammar::module_declaration>(
				GSL_TRACE_PARSE_OUTPUT,
				state.buffer,
				{.flags = lexy::visualize_fancy});
		#endif

		return state.mod;
	}
}
