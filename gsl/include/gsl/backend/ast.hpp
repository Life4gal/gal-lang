#pragma once

#include <gsl/string/string.hpp>
#include <gsl/string/string_view.hpp>
#include <gsl/container/vector.hpp>
#include <gsl/container/unordered_map.hpp>
#include <gsl/memory/memory.hpp>
#include <gsl/utility/utility.hpp>

namespace gal::gsl::ast
{
	using symbol_name = string::string;
	using symbol_name_view = std::string_view;

	class TypeDeclaration;
	class Variable;
	class Structure;
	class Function;
	class Expression;
	class Module;

	using type_declaration_type = memory::shared_ptr<TypeDeclaration>;
	using variable_type = memory::shared_ptr<Variable>;
	using structure_type = memory::shared_ptr<Structure>;
	using function_type = memory::shared_ptr<Function>;
	using expression_type = memory::shared_ptr<Expression>;
	using module_type = memory::shared_ptr<Module>;

	class TypeDeclaration final
	{
	public:
		enum class variable_type
		{
			NIL,

			VOID,
			BOOLEAN,
			INT,
			FLOAT,
			DOUBLE,
			STRING,

			STRUCTURE,
		};

		using dimension_type = std::uint32_t;
		using dimension_container_type = container::vector<dimension_type>;

		// parse failed(not builtin) -> return NIL
		[[nodiscard]] static auto parse_type(symbol_name_view name) -> variable_type;

	private:
		variable_type type_;
		// if not builtin
		Structure* owner_;
		// type[1][2][3][4]...
		dimension_container_type dimensions_;

	public:
		explicit TypeDeclaration(
				const variable_type type = variable_type::VOID,
				Structure* owner = nullptr,
				dimension_container_type&& dimensions = {}
				)
			: type_{type},
			owner_{owner},
			dimensions_{std::move(dimensions)} {}

		[[nodiscard]] constexpr auto type() const noexcept -> variable_type { return type_; }
	};

	class Variable final
	{
	public:
		struct variable_declaration
		{
			symbol_name name;
			type_declaration_type type;
		};

	private:
		variable_declaration declaration_;
		expression_type expression_;

	public:
		explicit Variable(
				variable_declaration&& declaration,
				expression_type&& expression = nullptr)
			: declaration_{std::move(declaration)},
			expression_{std::move(expression)} {}

		Variable(
				symbol_name&& name,
				type_declaration_type&& type,
				expression_type&& expression = nullptr)
			: Variable{
					variable_declaration{.name = std::move(name), .type = std::move(type)},
					std::move(expression)
			} {}

		Variable(
				const symbol_name_view name,
				type_declaration_type&& type,
				expression_type&& expression = nullptr
				)
			: Variable{symbol_name{name}, std::move(type), std::move(expression)} {}

		explicit Variable(symbol_name&& name)
			: Variable{std::move(name), {}} {}

		explicit Variable(const symbol_name_view name)
			: Variable{name, {}} {}

		[[nodiscard]] auto get_name() const noexcept -> symbol_name_view { return declaration_.name; }

		[[nodiscard]] auto get_type() const noexcept -> type_declaration_type { return declaration_.type; }

		auto set_type(type_declaration_type&& type) -> void { declaration_.type = std::move(type); }
		auto set_type(const type_declaration_type& type) -> void { declaration_.type = type; }

		auto set_expression(expression_type&& expression) -> void { expression_ = std::move(expression); }
		auto set_expression(const expression_type& expression) -> void { expression_ = expression; }
	};

	class Structure final
	{
	public:
		struct field_declaration;

		using field_container_type = container::vector<field_declaration>;
		using field_offset_type = field_container_type::size_type;

		struct field_declaration
		{
			Variable::variable_declaration variable;
			field_offset_type index;
		};

	private:
		symbol_name name_;
		field_container_type fields_;

	public:
		explicit Structure(const symbol_name_view name)
			: name_{name} {}

		[[nodiscard]] auto get_name() const -> symbol_name_view { return name_; }

		// this functions do not move from rvalue arguments if the insertion does not happen
		auto register_field(symbol_name&& name, type_declaration_type&& type) -> bool;
		// this functions do not move from rvalue arguments if the insertion does not happen
		auto register_field(Variable::variable_declaration&& variable) -> bool;

		auto register_field(symbol_name_view name, const type_declaration_type& type) -> bool;
	};

	class Function
	{
	public:
		using arguments_container_type = container::vector<variable_type>;

	private:
		symbol_name name_;
		arguments_container_type arguments_;
		type_declaration_type return_type_;
		expression_type function_body_;

	public:
		explicit Function(
				symbol_name&& name,
				arguments_container_type&& arguments = {},
				type_declaration_type&& return_type = {})
			: name_{std::move(name)},
			arguments_{std::move(arguments)},
			return_type_{std::move(return_type)} {}

		explicit Function(
				const symbol_name_view name,
				arguments_container_type&& arguments = {},
				type_declaration_type&& return_type = {}
				)
			: Function{symbol_name{name}, std::move(arguments), std::move(return_type)} {}

		Function(const Function&) = delete;
		auto operator=(const Function&) -> Function& = delete;
		Function(Function&&) = default;
		auto operator=(Function&&) -> Function& = default;
		virtual ~Function() noexcept;

		[[nodiscard]] constexpr virtual auto is_builtin() const noexcept -> bool { return false; }

		[[nodiscard]] auto get_name() const -> symbol_name_view { return name_; }

		auto set_arguments(arguments_container_type&& arguments) -> void { arguments_ = std::move(arguments); }

		auto set_return_type(type_declaration_type return_type) -> void { return_type_ = std::move(return_type); }

		auto set_function_body(expression_type&& function_body) -> void { function_body_ = std::move(function_body); }
	};

	class Expression
	{
	public:
	private:
		type_declaration_type type_;
	};

	// todo: EXPRESSIONS

	class Module : public memory::enable_shared_from_this<Module>
	{
	public:
		template<typename T>
		using symbol_table_type = container::unordered_map<symbol_name, T, utility::string_hasher<symbol_name>>;

	private:
		symbol_name name_;
		symbol_table_type<structure_type> structures_;
		symbol_table_type<variable_type> globals_;
		symbol_table_type<function_type> functions_;

	public:
		explicit Module(symbol_name&& name)
			: name_{std::move(name)} {}

		explicit Module(const symbol_name_view name)
			: name_{name} {}

		[[nodiscard]] auto get_name() const -> symbol_name_view { return name_; }

		// try_emplace: Unlike insert or emplace, this functions do not move from rvalue arguments if the insertion does not happen
		[[nodiscard]] auto register_structure(symbol_name&& name) -> std::pair<bool, structure_type>;
		[[nodiscard]] auto register_structure(const symbol_name_view name) -> std::pair<bool, structure_type> { return register_structure(symbol_name{name}); }

		// try_emplace: Unlike insert or emplace, this functions do not move from rvalue arguments if the insertion does not happen
		[[nodiscard]] auto register_global_mutable(symbol_name&& name) -> std::pair<bool, variable_type>;
		[[nodiscard]] auto register_global_mutable(const symbol_name_view name) -> std::pair<bool, variable_type> { return register_global_mutable(symbol_name{name}); }

		// try_emplace: Unlike insert or emplace, this functions do not move from rvalue arguments if the insertion does not happen
		[[nodiscard]] auto register_global_immutable(symbol_name&& name) -> std::pair<bool, variable_type>;
		[[nodiscard]] auto register_global_immutable(const symbol_name_view name) -> std::pair<bool, variable_type> { return register_global_immutable(symbol_name{name}); }

		// try_emplace: Unlike insert or emplace, this functions do not move from rvalue arguments if the insertion does not happen
		[[nodiscard]] auto register_function(symbol_name&& name) -> std::pair<bool, function_type>;
		[[nodiscard]] auto register_function(const symbol_name_view name) -> std::pair<bool, function_type> { return register_function(symbol_name{name}); }

		[[nodiscard]] auto has_structure(const symbol_name_view name) const -> bool { return structures_.contains(name); }

		[[nodiscard]] auto get_structure(const symbol_name_view name) const -> structure_type
		{
			if (const auto it = structures_.find(name);
				it != structures_.end()) { return it->second; }
			return nullptr;
		}

		[[nodiscard]] auto has_global(const symbol_name_view name) const -> bool { return globals_.contains(name); }

		[[nodiscard]] auto get_global(const symbol_name_view name) const -> variable_type
		{
			if (const auto it = globals_.find(name);
				it != globals_.end()) { return it->second; }
			return nullptr;
		}

		[[nodiscard]] auto has_function(const symbol_name_view name) const -> bool { return functions_.contains(name); }

		[[nodiscard]] auto get_function(const symbol_name_view name) const -> function_type
		{
			if (const auto it = functions_.find(name);
				it != functions_.end()) { return it->second; }
			return nullptr;
		}
	};
}
