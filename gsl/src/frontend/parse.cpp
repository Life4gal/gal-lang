#include <gsl/frontend/parse.hpp>
#include <gsl/backend/ast.hpp>

#include <lexy/dsl.hpp>
#include <lexy/action/parse.hpp>
#include <lexy/input/file.hpp>
#include <lexy/input/string_input.hpp>
#include <lexy_ext/report_error.hpp>
#include <lexy/callback.hpp>
#include <lexy/visualize.hpp>

#include <optional>
#include <cstdio>

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

			const auto out = lexy::cfile_output_iterator{stderr};
			const lexy_ext::diagnostic_writer writer{buffer, {.flags = lexy::visualize_fancy}};

			(void)writer.write_message(out,
										lexy_ext::diagnostic_kind::error,
										[&](lexy::cfile_output_iterator, lexy::visualization_options)
										{
											(void)std::fprintf(stderr, "unknown %s name '%s'", category, identifier.data());
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
						(void)std::fprintf(stderr, "used here");
						return out;
					});
		}

		auto report_duplicate_declaration(const char_type* position, const symbol_name_view identifier, const char* category) const -> void
		{
			const auto location = lexy::get_input_location(buffer, position, buffer_anchor);

			const auto out = lexy::cfile_output_iterator{stderr};
			const lexy_ext::diagnostic_writer writer{buffer, {.flags = lexy::visualize_fancy}};

			(void)writer.write_message(out,
										lexy_ext::diagnostic_kind::error,
										[&](lexy::cfile_output_iterator, lexy::visualization_options)
										{
											(void)std::fprintf(stderr, "duplicate %s declaration named '%s'", category, identifier.data());
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
						(void)std::fprintf(stderr, "second declaration here");
						return out;
					});
		}

		auto report_shadow_declaration(const char_type* position, const symbol_name_view identifier, const char* category) const -> void
		{
			const auto location = lexy::get_input_location(buffer, position, buffer_anchor);

			const auto out = lexy::cfile_output_iterator{stderr};
			const lexy_ext::diagnostic_writer writer{buffer, {.flags = lexy::visualize_fancy}};

			(void)writer.write_message(out,
										lexy_ext::diagnostic_kind::warning,
										[&](lexy::cfile_output_iterator, lexy::visualization_options)
										{
											(void)std::fprintf(stderr, "shadow %s declaration named '%s'", category, identifier.data());
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
						(void)std::fprintf(stderr, "second declaration here");
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

			constexpr static auto rule = dsl::list(dsl::lit_c<'['>//
													>> (dsl::integer<dimension_type> + dsl::lit_c<']'>));

			constexpr static auto value =
					lexy::fold_inplace<dimension_container_type>(
							[] { return dimension_container_type{}; },
							[](dimension_container_type& dimensions, const dimension_type i) -> void { dimensions.push_back(i); });
		};

		constexpr static auto rule =
				dsl::position +
				// type
				dsl::p<identifier> +
				// dimensions
				dsl::opt(
						dsl::peek(dsl::lit_c<'['>)
						>> dsl::p<dimension_list>);

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

					// todo: make type?
					return gsl::memory::make_shared<gsl::ast::TypeDeclaration>(
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

					// todo: make type?
					return gsl::memory::make_shared<gsl::ast::TypeDeclaration>(
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
				dsl::position +
				dsl::p<type_declaration> +
				dsl::p<identifier>;

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

	struct expression
	{
		// todo
		constexpr static auto rule = dsl::p<identifier>;

		// todo
		constexpr static auto value = ParseState::callback<gsl::ast::expression_type>(
				[](const ParseState& state, symbol_name&&) -> gsl::ast::expression_type
				{
					(void)state;
					return gsl::memory::make_shared<gsl::ast::Expression>();
				});
	};

	struct variable_declaration_with_assignment
	{
		constexpr static auto rule = dsl::p<variable_declaration> + dsl::equal_sign + dsl::p<expression>;

		constexpr static auto value = ParseState::callback<gsl::ast::variable_type>(
				[](const ParseState& state, gsl::ast::Variable::variable_declaration&& variable_declaration, gsl::ast::expression_type&& expression) -> gsl::ast::variable_type
				{
					(void)state;
					// todo: make variable?
					return gsl::memory::make_shared<gsl::ast::Variable>(
							std::move(variable_declaration),
							std::move(expression)
							);
				});
	};

	struct variable_declaration_with_optional_assign
	{
		constexpr static auto rule = dsl::p<variable_declaration> + dsl::opt(dsl::equal_sign >> dsl::p<expression>);

		constexpr static auto value = ParseState::callback<gsl::ast::variable_type>(
				// with assignment
				[](const ParseState& state, gsl::ast::Variable::variable_declaration&& variable_declaration, gsl::ast::expression_type&& expression) -> gsl::ast::variable_type
				{
					(void)state;
					// todo: make variable?
					return gsl::memory::make_shared<gsl::ast::Variable>(
							std::move(variable_declaration),
							std::move(expression));
				},
				// without assignment
				[](const ParseState& state, gsl::ast::Variable::variable_declaration&& variable_declaration, lexy::nullopt) -> gsl::ast::variable_type
				{
					(void)state;
					// todo: make variable?
					return gsl::memory::make_shared<gsl::ast::Variable>(std::move(variable_declaration));
				});
	};

	struct structure_declaration
	{
		[[nodiscard]] consteval static auto name() noexcept { return "structure declaration"; }

		// struct name
		struct header
		{
			constexpr static auto rule = dsl::position + dsl::p<identifier>;

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
			constexpr static auto rule = dsl::position + dsl::p<variable_declaration>;

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
				(dsl::p<header> +
				// todo: forward declaration?
				dsl::curly_bracketed.opt_list(dsl::p<field>));

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
				return LEXY_KEYWORD("mut", identifier::rule) >> dsl::position + dsl::p<variable_declaration_with_optional_assign>;
			}();

			constexpr static auto value = ParseState::callback<void>(
					[](const ParseState& state, const ParseState::char_type* position, gsl::ast::variable_type&& variable) -> void
					{
						// try register
						auto [success, v] = state.mod->register_global_immutable(variable->get_name());
						if (!success) { state.report_duplicate_declaration(position, v->get_name(), "global"); }

						// register succeeded, reset the pointer
						// Note: if the global variables are duplicate defined, the original global variables will be overwritten
						v.swap(variable);
					});
		};

		struct immutable_global
		{
			[[nodiscard]] consteval static auto name() noexcept { return "global immutable variable declaration"; }

			static constexpr auto rule = []
			{
				// [type variable] [= expression]
				// mutable variable may not have initializer
				return dsl::else_ >> dsl::position + dsl::p<variable_declaration_with_assignment>;
			}();

			static constexpr auto value = ParseState::callback<void>(
					[](const ParseState& state, const ParseState::char_type* position, gsl::ast::variable_type&& variable) -> void
					{
						// try register
						auto [success, v] = state.mod->register_global_immutable(variable->get_name());
						if (!success) { state.report_duplicate_declaration(position, v->get_name(), "global"); }

						// register succeeded, reset the pointer
						// Note: if the global variables are duplicate defined, the original global variables will be overwritten
						v.swap(variable);
					}
					);
		};

		constexpr static auto rule =
				LEXY_KEYWORD("global", identifier::rule) >>
				(dsl::p<mutable_global> | dsl::p<immutable_global>) +
				// semicolon required ?
				dsl::semicolon;

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
					dsl::p<identifier> +
					// arguments
					dsl::p<arguments_list> +
					// return type
					// todo: optional return type?
					LEXY_LIT("->").error<return_type_expected> +
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
			constexpr static auto rule = dsl::curly_bracketed.open() >> dsl::p<expression> + dsl::curly_bracketed.close();

			constexpr static auto value = ParseState::callback<void>(
					[](const ParseState& state, gsl::ast::expression_type&& body) -> void { state.current_function->set_function_body(std::move(body)); }
					);
		};

		constexpr static auto rule =
				LEXY_KEYWORD("fn", identifier::rule) >>
				// function declaration
				dsl::p<header> +
				// function body
				dsl::p<body>;

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
					dsl::p<identifier> +
					// semicolon required ?
					dsl::semicolon;

			constexpr static auto value = ParseState::callback<void>(
					[](ParseState& state, symbol_name&& symbol) -> void
					{
						// todo: make module?
						state.mod = gsl::memory::make_shared<gsl::ast::Module>(std::move(symbol));
					});
		};

		constexpr static auto rule =
				dsl::p<header> +
				dsl::terminator(dsl::eof).opt_list(
						dsl::p<global_declaration> |
						dsl::p<function_declaration>
						);

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
			throw std::exception{"Cannot read file!"};
		}

		ParseState state{string::string{filename}, std::move(file).buffer()};

		if (const auto result = lexy::parse<grammar::module_declaration>(state.buffer, state, lexy_ext::report_error.opts({.flags = lexy::visualize_fancy}).path(state.filename.c_str()));
			!result.is_success())
		{
			// todo: handle it?
			throw std::exception{"Cannot parse file!"};
		}

		return state.mod;
	}
}
