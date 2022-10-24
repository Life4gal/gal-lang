#include <gsl/parse/parser.hpp>

#include <lexy/action/parse.hpp> // lexy::parse
#include <lexy/callback.hpp>     // value callbacks
#include <lexy/dsl.hpp>          // lexy::dsl::*
#include <lexy/input/file.hpp>   // lexy::read_file
#include <lexy_ext/report_error.hpp> // lexy_ext::report_error

#include <exception>
#include <iostream>

namespace
{
	using namespace gal::gsl;
	namespace dsl = lexy::dsl;

	namespace parser
	{
		struct comment : public lexy::token_production
		{
			struct single_line : public lexy::transparent_production
			{
				constexpr static auto rule = dsl::delimited(dsl::lit_c<'#'>, dsl::newline)(dsl::code_point);
				constexpr static auto value = lexy::as_string<parse::node_comment::value_type, lexy::utf8_encoding>;
			};

			struct multi_line_single_quotation : public lexy::transparent_production
			{
				constexpr static auto rule = dsl::delimited(LEXY_LIT("'''"), LEXY_LIT("'''"))(dsl::code_point);
				constexpr static auto value = lexy::as_string<parse::node_comment::value_type, lexy::utf8_encoding>;
			};

			struct multi_line_double_quotation : public lexy::transparent_production
			{
				constexpr static auto rule = dsl::delimited(LEXY_LIT("\"\"\""), LEXY_LIT("\"\"\""))(dsl::code_point);
				constexpr static auto value = lexy::as_string<parse::node_comment::value_type, lexy::utf8_encoding>;
			};

			constexpr static auto rule = dsl::p<single_line> | dsl::p<multi_line_single_quotation> | dsl::p<multi_line_double_quotation>;

			constexpr static auto value = lexy::construct<parse::node_comment>;
		};

		struct boolean : public lexy::token_production
		{
			struct true_part : lexy::transparent_production
			{
				constexpr static auto rule = LEXY_LIT("true");
				constexpr static auto value = lexy::constant(true);
			};

			struct false_part : lexy::transparent_production
			{
				constexpr static auto rule = LEXY_LIT("false");
				constexpr static auto value = lexy::constant(false);
			};

			constexpr static auto rule = dsl::p<true_part> | dsl::p<false_part>;
			constexpr static auto value = lexy::construct<parse::node_bool>;
		};

		struct number : public lexy::token_production
		{
			struct value_part : lexy::transparent_production
			{
				struct invalid_digit
				{
					consteval static auto name() { return "invalid floating point digit"; }
				};

				constexpr static auto rule = []
				{
					// todo: sign
					constexpr auto optional_sign = dsl::sign;
					constexpr auto digits = dsl::digit<>.error<invalid_digit>;

					return dsl::identifier(digits);
				}();

				constexpr static auto value = lexy::as_string<parse::node_number::value_type, lexy::utf8_encoding>;
			};

			struct fraction_part : lexy::transparent_production
			{
				constexpr static auto rule = dsl::lit_c<'.'> >> dsl::capture(dsl::digits<>);

				constexpr static auto value = lexy::as_string<parse::node_number::fraction_type, lexy::utf8_encoding>;
			};

			struct exponent_part : lexy::transparent_production
			{
				constexpr static auto rule = (dsl::lit_c<'e'> | dsl::lit_c<'E'>) >> dsl::sign + dsl::integer<parse::node_number::exponent_type>;

				constexpr static auto value = lexy::as_integer<parse::node_number::exponent_type>;
			};

			// peek '-' or digit
			// + optional fraction_part and exponent_part
			constexpr static auto rule = dsl::peek(dsl::lit_c<'-'> / dsl::digit<>) >> dsl::p<value_part> + dsl::opt(dsl::p<fraction_part>) + dsl::opt(dsl::p<exponent_part>);

			constexpr static auto value = lexy::construct<parse::node_number>;
		};

		struct string : public lexy::token_production
		{
			struct value_part : public lexy::transparent_production
			{
				struct invalid_character
				{
					consteval static auto name() noexcept { return "invalid character in string literal"; }
				};

				// A mapping of the simple escape sequences to their replacement values.
				constexpr static auto escaped_symbols = lexy::symbol_table<char>//
														.map<'"'>('"')
														.map<'\\'>('\\')
														.map<'/'>('/')
														.map<'b'>('\b')
														.map<'f'>('\f')
														.map<'n'>('\n')
														.map<'r'>('\r')
														.map<'t'>('\t');

				// Unicode code point can be specified by its encoding in UTF-16:
				// * code points <= 0xFFFF are specified using `\uNNNN`.
				// * other code points are specified by two surrogate UTF-16 sequences.
				// However, we don't combine the two surrogates into the final code point,
				// instead keep them separate and require a later pass that merges them if necessary.
				// (This behavior is allowed by the standard).
				struct code_point_id
				{
					// We parse the integer value of a UTF-16 code unit.
					constexpr static auto rule = LEXY_LIT("u") >> dsl::code_unit_id<lexy::utf16_encoding, 4>;
					// And convert it into a code point, which might be a surrogate.
					constexpr static auto value = lexy::construct<lexy::code_point>;
				};

				constexpr static auto rule = []
				{
					// Everything is allowed inside a string except for control characters.
					constexpr auto code_point = (-dsl::unicode::control).error<invalid_character>;

					// Escape sequences start with a backlash and either map one of the symbols,
					// or a Unicode code point.
					constexpr auto escape = dsl::backslash_escape.symbol<escaped_symbols>().rule(dsl::p<code_point_id>);

					// String of code_point with specified escape sequences, surrounded by ".
					// We abort string parsing if we see a `newline` to handle missing closing ".
					// todo: allow multi-line string
					return dsl::quoted.limit(dsl::ascii::newline)(code_point, escape);
				}();

				constexpr static auto value = lexy::as_string<parse::node_string::value_type, lexy::utf8_encoding>;
			};

			constexpr static auto rule = dsl::p<value_part>;

			constexpr static auto value = lexy::construct<parse::node_string>;
		};

		struct node : public lexy::token_production
		{
			struct expected_node
			{
				consteval static auto name() noexcept { return "expected node here"; }
			};

			constexpr static auto rule = []
			{
				constexpr auto primitive =
						dsl::p<boolean> |
						dsl::p<number> |
						dsl::p<string>;
				// constexpr auto complex	 = dsl::p<>;

				return dsl::p<comment> | primitive | /* complex | */ dsl::error<expected_node>;
			}();

			constexpr static auto value = lexy::construct<parse::Node>;
		};

		// ===================================
		// =========== PRODUCTION ============
		// ===================================
		struct production
		{
			constexpr static auto name = "production";

			// todo
			constexpr static auto max_recursion_depth = 32;

			// Whitespace is a sequence of space, tab, carriage return, or newline.
			constexpr static auto whitespace = dsl::ascii::space;

			struct node_tree_part
			{
				constexpr static auto rule = dsl::brackets(
						LEXY_LIT("BEGIN"),
						LEXY_LIT("END")
						).opt_list(
						dsl::recurse<node>//,
						//dsl::sep(dsl::eol)
						);

				constexpr static auto value = lexy::as_list<parse::node_tree_result>;
			};

			constexpr static auto rule = dsl::p<node_tree_part>;

			constexpr static auto value = lexy::construct<parse::node_tree_result>;
		};
	}
}

namespace gal::gsl::parse
{
	auto parse_file(const string::string_view filename) -> node_tree_result
	{
		const auto file = lexy::read_file<lexy::utf8_encoding>(filename.data());
		if (!file)
		{
			// todo
			throw std::exception{"Cannot read file!"};
		}

		auto production = lexy::parse<parser::production>(file.buffer(), lexy_ext::report_error);
		if (!production.has_value())
		{
			// todo
			throw std::exception{"Context error!"};
		}

		return std::move(production).value();
	}
}
