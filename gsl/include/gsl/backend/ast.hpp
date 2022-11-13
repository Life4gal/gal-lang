#pragma once

#include <gsl/string/string.hpp>
#include <gsl/string/string_view.hpp>
#include <gsl/container/vector.hpp>
#include <gsl/container/unordered_map.hpp>
#include <gsl/memory/memory.hpp>
#include <gsl/utility/utility.hpp>

#include <optional>
#include <ranges>

namespace gal::gsl::simulate
{
	class SimulateNode;
	class SimulateContext;
}

namespace gal::gsl::ast
{
	using symbol_name = string::string;
	using symbol_name_view = std::string_view;

	class Visitor;

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

	template<typename T, typename... Args>
	[[nodiscard]] constexpr auto make(Args&&... args) -> memory::shared_ptr<T> { return memory::make_shared<T>(std::forward<Args>(args)...); }

	class Visitor
	{
	public:
		using string_view_type = string::string_view;

		Visitor() = default;
		Visitor(const Visitor&) = delete;
		Visitor& operator=(const Visitor&) = delete;
		Visitor(Visitor&&) noexcept = default;
		Visitor& operator=(Visitor&&) noexcept = default;
		virtual ~Visitor() noexcept = default;

		virtual void log(string_view_type message) = 0;

		virtual void visit(const Expression& expression) = 0;
	};

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
		bool is_reference_;

	public:
		explicit TypeDeclaration(
				const variable_type type = variable_type::VOID,
				Structure* owner = nullptr,
				dimension_container_type&& dimensions = {},
				const bool is_reference = false
				)
			: type_{type},
			  owner_{owner},
			  dimensions_{std::move(dimensions)},
			  is_reference_{is_reference} {}

		[[nodiscard]] constexpr auto type() const noexcept -> variable_type { return type_; }

		[[nodiscard]] auto type_name() const noexcept -> symbol_name_view;

		[[nodiscard]] constexpr auto dimensions() const noexcept { return dimensions_ | std::views::all; }

		[[nodiscard]] constexpr auto is_reference() const noexcept { return is_reference_; }

		void log(Visitor& visitor) const;
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

		[[nodiscard]] constexpr auto get_declaration() const noexcept -> const variable_declaration& { return declaration_; }

		[[nodiscard]] constexpr auto get_name() const noexcept -> symbol_name_view { return declaration_.name; }

		[[nodiscard]] auto get_type() const noexcept -> type_declaration_type { return declaration_.type; }

		[[nodiscard]] auto get_expression() const noexcept -> expression_type { return expression_; }

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

		[[nodiscard]] constexpr auto fields() const noexcept { return fields_ | std::views::all; }

		// this functions do not move from rvalue arguments if the insertion does not happen
		auto register_field(symbol_name&& name, type_declaration_type&& type) -> bool;
		// this functions do not move from rvalue arguments if the insertion does not happen
		auto register_field(Variable::variable_declaration&& variable) -> bool;

		auto register_field(symbol_name_view name, const type_declaration_type& type) -> bool;

		void log(Visitor& visitor) const;
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

		[[nodiscard]] constexpr auto get_name() const -> symbol_name_view { return name_; }

		[[nodiscard]] constexpr auto get_arguments() const noexcept
		{
			return arguments_ |
			       std::views::all |
			       std::views::transform([](const auto& argument) -> const Variable& { return *argument; });
		}

		[[nodiscard]] auto get_return_type() const noexcept -> type_declaration_type { return return_type_; }

		[[nodiscard]] auto get_function_body() const noexcept -> expression_type { return function_body_; }

		auto set_arguments(arguments_container_type&& arguments) -> void { arguments_ = std::move(arguments); }

		auto set_return_type(type_declaration_type return_type) -> void { return_type_ = std::move(return_type); }

		auto set_function_body(expression_type&& function_body) -> void { function_body_ = std::move(function_body); }

		void log(Visitor& visitor) const;

		virtual auto make_simulate_node(simulate::SimulateContext& context) -> simulate::SimulateNode*;
	};

	class Expression
	{
	public:
		// todo
		struct infer_type_context
		{
			module_type mod;
			function_type function;
			container::vector<variable_type> local_variables;
		};

	protected:
		type_declaration_type type_;

		explicit Expression(type_declaration_type&& type)
			: type_{std::move(type)} {}

		// todo: empty?
		Expression() = default;

		Expression(const Expression&) = delete;
		Expression& operator=(const Expression&) = delete;
		Expression(Expression&&) noexcept = default;
		Expression& operator=(Expression&&) noexcept = default;

		virtual ~Expression() noexcept = default;

	public:
		[[nodiscard]] virtual auto simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode* = 0;

		[[nodiscard]] virtual auto deep_copy_from_this(expression_type dest) const -> expression_type;
		[[nodiscard]] virtual auto deep_copy_from_this() const -> expression_type;

		virtual void visit(Visitor& visitor) const { visitor.visit(*this); }
	};

	class ExpressionNoop final : public Expression
	{
		auto simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode* override
		{
			// todo
			(void)context;
			return nullptr;
		}
	};

	class ExpressionOperator : public Expression
	{
	public:
	protected:
		function_type function_{nullptr};// always built-in function

		ExpressionOperator() = default;

	public:
		[[nodiscard]] auto deep_copy_from_this(expression_type dest) const -> expression_type override;
		[[nodiscard]] auto deep_copy_from_this() const -> expression_type override;
	};

	class ExpressionUnaryOperator final : public ExpressionOperator
	{
	public:
		enum class operand_type
		{
			// -x
			NEGATE,
			// ~x
			BIT_COMPLEMENT,
			// !x
			LOGICAL_NOT
		};

	private:
		operand_type operand_;
		expression_type rhs_;

	public:
		ExpressionUnaryOperator(
				const operand_type operand,
				expression_type&& rhs)
			: operand_{operand},
			  rhs_{std::move(rhs)} {}

		// todo: invalid
		ExpressionUnaryOperator() = default;

		auto simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode* override;

		[[nodiscard]] auto deep_copy_from_this(expression_type dest) const -> expression_type override;
		[[nodiscard]] auto deep_copy_from_this() const -> expression_type override;
	};

	class ExpressionBinaryOperator final : public ExpressionOperator
	{
	public:
		enum class operand_type
		{
			PLUS,
			MINUS,
			MULTIPLY,
			DIVIDE,
			POW,

			BIT_AND,
			BIT_OR,
			BIT_XOR,

			EQUAL,
			GREATER_EQUAL,
			GREATER,
			LESS_EQUAL,
			LESS,
		};

	private:
		operand_type operand_;
		expression_type lhs_;
		expression_type rhs_;

	public:
		ExpressionBinaryOperator(
				const operand_type operand,
				expression_type&& lhs,
				expression_type&& rhs
				)
			: operand_{operand},
			  lhs_{std::move(lhs)},
			  rhs_{std::move(rhs)} {}

		// todo: invalid
		ExpressionBinaryOperator() = default;

		auto simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode* override;

		[[nodiscard]] auto deep_copy_from_this(expression_type dest) const -> expression_type override;
		[[nodiscard]] auto deep_copy_from_this() const -> expression_type override;
	};


	class ExpressionConstant : public Expression
	{
	protected:
		using Expression::Expression;
	};

	class ExpressionConstantInt final : public ExpressionConstant
	{
	public:
		// todo: string?
		using value_type = std::int64_t;

	private:
		value_type value_;

	public:
		explicit ExpressionConstantInt(const value_type value = {})
			: value_{value} {}

		auto simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode* override;

		[[nodiscard]] auto deep_copy_from_this(expression_type dest) const -> expression_type override;
		[[nodiscard]] auto deep_copy_from_this() const -> expression_type override;
	};

	class ExpressionConstantFloat final : public ExpressionConstant
	{
	public:
		// todo
		using value_type = std::int64_t;
		using fractional_type = std::optional<std::int64_t>;
		using exponent_type = std::optional<std::int32_t>;

	private:
		value_type value_;
		fractional_type fractional_;
		exponent_type exponent_;

	public:
		explicit ExpressionConstantFloat(
				const value_type value = {},
				const fractional_type fractional = {},
				const exponent_type exponent = {}
				)
			: value_{value},
			  fractional_{fractional},
			  exponent_{exponent} {}

		auto simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode* override;

		[[nodiscard]] auto deep_copy_from_this(expression_type dest) const -> expression_type override;
		[[nodiscard]] auto deep_copy_from_this() const -> expression_type override;
	};

	class ExpressionConstantString final : public ExpressionConstant
	{
	public:
		using value_type = symbol_name;

	private:
		value_type value_;

	public:
		explicit ExpressionConstantString(
				value_type&& value = {})
			: value_{std::move(value)} {}

		auto simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode* override;

		[[nodiscard]] auto deep_copy_from_this(expression_type dest) const -> expression_type override;
		[[nodiscard]] auto deep_copy_from_this() const -> expression_type override;
	};

	class Module final : public memory::enable_shared_from_this<Module>
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
