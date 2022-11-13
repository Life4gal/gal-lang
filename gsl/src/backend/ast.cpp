#include <gsl/backend/ast.hpp>
#include <gsl/debug/assert.hpp>
#include <gsl/simulate/simulate.hpp>
#include <gsl/string/format.hpp>

#include <magic_enum.hpp>

#include <algorithm>
#include <stdexcept>

// format
namespace fmt
{
	namespace ast = gal::gsl::ast;
	namespace string = gal::gsl::string;

	template<>
	struct formatter<ast::TypeDeclaration>
	{
		template<typename ParseContext>
		constexpr auto parse(ParseContext& ctx) const { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const ast::TypeDeclaration& type_declaration, FormatContext& ctx) const
		{
			return string::format_to(
					ctx.out(),
					"{}{}{}",
					type_declaration.type_name(),
					fmt::join(type_declaration.dimensions(), "|"),
					type_declaration.is_reference() ? "&" : ""
					);
		}
	};

	template<>
	struct formatter<ast::Variable::variable_declaration>
	{
		template<typename ParseContext>
		constexpr auto parse(ParseContext& ctx) const { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const ast::Variable::variable_declaration& variable_declaration, FormatContext& ctx) const
		{
			return string::format_to(
					ctx.out(),
					"{}: {}",
					variable_declaration.name,
					*variable_declaration.type
					);
		}
	};

	template<>
	struct formatter<ast::Variable>
	{
		template<typename ParseContext>
		constexpr auto parse(ParseContext& ctx) const { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const ast::Variable& variable, FormatContext& ctx) const
		{
			// todo
			auto ret = string::format_to(
					ctx.out(),
					"{}",
					variable.get_declaration());

			if (const auto expression = variable.get_expression())
			{
				ret = string::format_to(
						ctx.out(),
						" = {}",
						*expression);
			}
			return ret;
		}
	};

	template<>
	struct formatter<ast::Structure::field_declaration> : public formatter<ast::Variable::variable_declaration>
	{
		template<typename ParseContext>
		constexpr auto parse(ParseContext& ctx) const { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const ast::Structure::field_declaration& field_declaration, FormatContext& ctx) const { return formatter<ast::Variable::variable_declaration>::format(field_declaration.variable, ctx); }
	};

	template<>
	struct formatter<ast::Structure>
	{
		template<typename ParseContext>
		constexpr auto parse(ParseContext& ctx) const { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const ast::Structure& s, FormatContext& ctx) const
		{
			return string::format_to(
					ctx.out(),
					"struct {}[\n\t{}\n]",
					s.get_name(),
					fmt::join(s.fields(), "\n\t")
					);
		}
	};

	template<>
	struct formatter<ast::Function>
	{
		template<typename ParseContext>
		constexpr auto parse(ParseContext& ctx) const { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const ast::Function& function, FormatContext& ctx) const
		{
			return string::format_to(
					ctx.out(),
					"function {} ({}) -> {}\n{}",
					function.get_name(),
					fmt::join(function.get_arguments(), ", "),
					*function.get_return_type(),
					*function.get_function_body()
					);
		}
	};

	template<>
	struct formatter<ast::Expression>
	{
		template<typename ParseContext>
		constexpr auto parse(ParseContext& ctx) const { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const ast::Expression& expression, FormatContext& ctx) const
		{
			// todo
			(void)expression;
			return string::format_to(
					ctx.out(),
					"{}",
					"expression");
		}
	};
}

namespace gal::gsl::ast
{
	auto TypeDeclaration::parse_type(const symbol_name_view name) -> variable_type
	{
		// todo: how?
		for (constexpr auto variable_entries = magic_enum::enum_entries<variable_type>();
		     const auto& [type, variable_name]: variable_entries)
		{
			if (std::ranges::equal(
					variable_name,
					name,
					std::ranges::equal_to{},
					[](const auto c) { return std::toupper(c); },
					[](const auto c) { return std::toupper(c); }
					)) { return type; }
		}

		return variable_type::NIL;
	}

	auto TypeDeclaration::type_name() const noexcept -> symbol_name_view { return type_ == variable_type::STRUCTURE ? owner_->get_name() : magic_enum::enum_name(type_); }

	void TypeDeclaration::log(Visitor& visitor) const
	{
		gsl_assert(!((type_ == variable_type::STRUCTURE) ^ (owner_ != nullptr)));

		visitor.log(string::format("{}", *this));
	}

	auto Structure::register_field(symbol_name&& name, type_declaration_type&& type) -> bool
	{
		if (const auto it = std::ranges::find(
					fields_,
					name,
					[](const auto& field) -> const symbol_name& { return field.variable.name; });
			it != fields_.end()) { return false; }

		fields_.emplace_back(Variable::variable_declaration{.name = std::move(name), .type = std::move(type)}, fields_.size());
		return true;
	}

	auto Structure::register_field(Variable::variable_declaration&& variable) -> bool
	{
		if (const auto it = std::ranges::find(
					fields_,
					variable.name,
					[](const auto& field) -> const symbol_name& { return field.variable.name; });
			it != fields_.end()) { return false; }

		fields_.emplace_back(std::move(variable));
		return true;
	}

	auto Structure::register_field(const symbol_name_view name, const type_declaration_type& type) -> bool
	{
		if (const auto it = std::ranges::find(
					fields_,
					name,
					[](const auto& field) -> symbol_name_view { return field.variable.name; });
			it != fields_.end()) { return false; }

		fields_.emplace_back(Variable::variable_declaration{.name = symbol_name{name}, .type = type}, fields_.size());
		return true;
	}

	void Structure::log(Visitor& visitor) const { visitor.log(string::format("{}", *this)); }

	Function::~Function() noexcept = default;

	void Function::log(Visitor& visitor) const { visitor.log(string::format("{}", *this)); }

	auto Function::make_simulate_node(simulate::SimulateContext& context) -> simulate::SimulateNode* { return context.make_node<simulate::SimulateFunctionCallNode>(); }

	template<typename ExpressionType, typename BaseType = Expression>
		requires std::is_base_of_v<BaseType, ExpressionType>
	[[nodiscard]] auto common_deep_copy_from_this(expression_type dest) -> memory::shared_ptr<ExpressionType>
	{
		auto ret = dest ? std::static_pointer_cast<ExpressionType>(dest) : make<ExpressionType>();
		(void)(*ret).BaseType::deep_copy_from_this(ret);
		return ret;
	}

	auto Expression::deep_copy_from_this(expression_type dest) const -> expression_type
	{
		gsl_assert(dest.operator bool());

		// deep copy
		dest->type_ = type_ ? make<TypeDeclaration>(*type_) : nullptr;

		return dest;
	}

	auto Expression::deep_copy_from_this() const -> expression_type { throw std::runtime_error{"unsupported expression"}; }

	auto ExpressionOperator::deep_copy_from_this(expression_type dest) const -> expression_type
	{
		gsl_assert(dest.operator bool());

		auto op = std::static_pointer_cast<ExpressionOperator>(dest);
		gsl_assert(op.operator bool());
		op->function_ = function_;

		return op;
	}

	auto ExpressionOperator::deep_copy_from_this() const -> expression_type { throw std::runtime_error{"cannot copy ExpressionOperator"}; }

	auto ExpressionUnaryOperator::simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode*
	{
		auto* node = dynamic_cast<simulate::SimulateUnaryOperatorNodeBase*>(function_->make_simulate_node(context));

		gsl_assert(node);
		node->set_simulate_node(rhs_->simulate(context));
		return node;
	}

	auto ExpressionUnaryOperator::deep_copy_from_this(expression_type dest) const -> expression_type
	{
		auto op = common_deep_copy_from_this<ExpressionUnaryOperator, ExpressionOperator>(dest);

		// set operand and expression
		op->operand_ = operand_;
		op->rhs_ = rhs_->deep_copy_from_this();
		return op;
	}

	auto ExpressionUnaryOperator::deep_copy_from_this() const -> expression_type
	{
		auto op = common_deep_copy_from_this<ExpressionUnaryOperator, ExpressionOperator>(nullptr);

		// set operand and expression
		op->operand_ = operand_;
		op->rhs_ = rhs_->deep_copy_from_this();
		return op;
	}

	auto ExpressionBinaryOperator::simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode*
	{
		auto* node = dynamic_cast<simulate::SimulateBinaryOperatorNodeBase*>(function_->make_simulate_node(context));

		gsl_assert(node);
		node->set_lhs_simulate_node(lhs_->simulate(context));
		node->set_rhs_simulate_node(rhs_->simulate(context));
		return node;
	}

	auto ExpressionBinaryOperator::deep_copy_from_this(expression_type dest) const -> expression_type
	{
		auto op = common_deep_copy_from_this<ExpressionBinaryOperator, ExpressionOperator>(dest);

		// set operand and expression
		op->operand_ = operand_;
		op->lhs_ = lhs_->deep_copy_from_this();
		op->rhs_ = rhs_->deep_copy_from_this();
		return op;
	}

	auto ExpressionBinaryOperator::deep_copy_from_this() const -> expression_type
	{
		auto op = common_deep_copy_from_this<ExpressionBinaryOperator, ExpressionOperator>(nullptr);

		// set operand and expression
		op->operand_ = operand_;
		op->lhs_ = lhs_->deep_copy_from_this();
		op->rhs_ = rhs_->deep_copy_from_this();
		return op;
	}

	auto ExpressionConstantInt::simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode* { return context.make_node<simulate::SimulateConstantNode<value_type>>(value_); }

	auto ExpressionConstantInt::deep_copy_from_this(expression_type dest) const -> expression_type
	{
		auto op = common_deep_copy_from_this<ExpressionConstantInt, ExpressionConstant>(dest);

		// set value
		op->value_ = value_;
		return op;
	}

	auto ExpressionConstantInt::deep_copy_from_this() const -> expression_type
	{
		auto op = common_deep_copy_from_this<ExpressionConstantInt, ExpressionConstant>(nullptr);

		// set value
		op->value_ = value_;
		return op;
	}

	auto ExpressionConstantFloat::simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode*
	{
		// todo: how can type::Value store the entire value?
		return context.make_node<simulate::SimulateConstantNode<value_type>>(value_);
	}

	auto ExpressionConstantFloat::deep_copy_from_this(expression_type dest) const -> expression_type
	{
		auto op = common_deep_copy_from_this<ExpressionConstantFloat, ExpressionConstant>(dest);

		// set value
		op->value_ = value_;
		return op;
	}

	auto ExpressionConstantFloat::deep_copy_from_this() const -> expression_type
	{
		auto op = common_deep_copy_from_this<ExpressionConstantFloat, ExpressionConstant>(nullptr);

		// set value
		op->value_ = value_;
		return op;
	}

	auto ExpressionConstantString::simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode*
	{
		// todo: move it?
		// return context.make_node<simulate::SimulateConstantNode<value_type>>(value_);
		// todo: implement it
		(void)context;
		return nullptr;
	}

	auto ExpressionConstantString::deep_copy_from_this(expression_type dest) const -> expression_type
	{
		auto op = common_deep_copy_from_this<ExpressionConstantString, ExpressionConstant>(dest);

		// set value
		op->value_ = value_;
		return op;
	}

	auto ExpressionConstantString::deep_copy_from_this() const -> expression_type
	{
		auto op = common_deep_copy_from_this<ExpressionConstantString, ExpressionConstant>(nullptr);

		// set value
		op->value_ = value_;
		return op;
	}

	auto Module::register_structure(symbol_name&& name) -> std::pair<bool, structure_type>
	{
		if (const auto it = structures_.find(name);
			it != structures_.end()) { return std::make_pair(false, it->second); }

		// use name first
		auto structure = memory::make_shared<Structure>(name);
		// move it
		auto [it, inserted] = structures_.try_emplace(
				std::move(name),
				std::move(structure)
				);

		gsl_assert(inserted, "impossible happened!");
		return std::make_pair(true, it->second);
	}

	auto Module::register_global_mutable(symbol_name&& name) -> std::pair<bool, variable_type>
	{
		if (const auto it = globals_.find(name);
			it != globals_.end()) { return std::make_pair(false, it->second); }

		// use name first
		auto global = memory::make_shared<Variable>(name);
		// move it
		auto [it, inserted] = globals_.try_emplace(
				std::move(name),
				std::move(global));

		gsl_assert(inserted, "impossible happened!");
		return std::make_pair(true, it->second);
	}

	auto Module::register_global_immutable(symbol_name&& name) -> std::pair<bool, variable_type>
	{
		// todo
		return register_global_mutable(std::move(name));
	}

	auto Module::register_function(symbol_name&& name) -> std::pair<bool, function_type>
	{
		if (const auto it = functions_.find(name);
			it != functions_.end()) { return std::make_pair(false, it->second); }

		// use name first
		auto function = memory::make_shared<Function>(name);
		// move it
		auto [it, inserted] = functions_.try_emplace(
				std::move(name),
				std::move(function));

		gsl_assert(inserted, "impossible happened!");
		return std::make_pair(true, it->second);
	}
}
