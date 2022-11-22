#include <gsl/backend/ast.hpp>
#include <gsl/debug/assert.hpp>
#include <gsl/simulate/simulate.hpp>
#include <gsl/string/format.hpp>
#include <gsl/utility/function.hpp>
#include <gsl/debug/debugger.hpp>

#include <magic_enum.hpp>

#include <algorithm>
#include <stdexcept>
#include <execution>
#include <utility>

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
					type_declaration.is_xvalue() ? "&" : ""
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

	template<>
	struct formatter<ast::ExpressionFunctionCall>
	{
		template<typename ParseContext>
		constexpr auto parse(ParseContext& ctx) const { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const ast::ExpressionFunctionCall& function_call, FormatContext& ctx) const
		{
			return string::format_to(
					ctx.out(),
					"call {} with {}",
					function_call.get_function(),
					fmt::join(function_call.get_arguments(), ", "));
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

	constexpr auto TypeDeclaration::size_without_dimensions() const noexcept -> type_size_type
	{
		switch (type_)
		{
			case variable_type::NIL:
			case variable_type::VOID:
			{
				// todo:
				return 0;
			}
			case variable_type::BOOLEAN: { return sizeof(ExpressionConstantBoolean::value_type); }
			case variable_type::INT: { return sizeof(ExpressionConstantInt::value_type); }
			case variable_type::FLOAT: { return sizeof(ExpressionConstantFloat::value_type); }
			case variable_type::STRING:
			{
				// todo
				return 0;
			}
			case variable_type::STRUCTURE:
			{
				gsl_assert(owner_);
				return owner_->structure_size();
			}
		}

		// std::unreachable();
		debug::break_here();
	}

	auto TypeDeclaration::size_with_dimensions() const noexcept -> type_size_type
	{
		return size_without_dimensions() * std::reduce(
				       dimensions_.begin(),
				       dimensions_.end(),
				       type_size_type{1},
				       std::multiplies<type_size_type>{});
	}

	void TypeDeclaration::log(Visitor& visitor) const
	{
		gsl_assert(!((type_ == variable_type::STRUCTURE) ^ (owner_ != nullptr)));

		visitor.log(string::format("{}", *this));
	}

	auto Structure::structure_size() const noexcept -> TypeDeclaration::type_size_type
	{
		const auto type_sizes =
				fields_ |
				std::views::transform(
						[](const auto& field) -> TypeDeclaration::type_size_type { return field.variable.type->size_with_dimensions(); });

		return std::reduce(
				type_sizes.begin(),
				type_sizes.end(),
				TypeDeclaration::type_size_type{0},
				std::plus<TypeDeclaration::type_size_type>{});
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

	auto Function::simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode*
	{
		if (is_builtin()) { throw std::runtime_error{"can only simulate non builtin function"}; }

		return function_body_->simulate(context);
	}

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
		dest->result_type_ = result_type_ ? make<TypeDeclaration>(*result_type_) : nullptr;

		return dest;
	}

	auto Expression::deep_copy_from_this() const -> expression_type { throw std::runtime_error{"unsupported expression"}; }

	auto ExpressionBlock::simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode*
	{
		auto* node = context.make_node<simulate::SimulateBlockNode>();

		simulate::SimulateBlockNode::expressions_type expressions{
				reinterpret_cast<simulate::SimulateBlockNode::expressions_type::pointer>(context.allocate(sizeof(simulate::SimulateNode*) * expressions_.size())),
				expressions_.size()};

		utility::zip_invoke(
				expressions.begin(),
				expressions.end(),
				[&context](auto& expression, const auto& to_simulate) { expression = to_simulate->simulate(context); },
				expressions_.begin());

		node->set_expressions(expressions);
		return node;
	}

	auto ExpressionBlock::infer_type(infer_type_context& context) -> void
	{
		// return void / no return
		result_type_ = make<TypeDeclaration>();
		for (const auto& expression: expressions_) { expression->infer_type(context); }
	}

	auto ExpressionBlock::deep_copy_from_this(expression_type dest) const -> expression_type
	{
		auto block = common_deep_copy_from_this<ExpressionBlock>(dest);

		for (auto& expression: expressions_) { block->expressions_.push_back(expression->deep_copy_from_this()); }

		return block;
	}

	auto ExpressionBlock::deep_copy_from_this() const -> expression_type
	{
		auto block = common_deep_copy_from_this<ExpressionBlock>(nullptr);

		for (auto& expression: expressions_) { block->expressions_.push_back(expression->deep_copy_from_this()); }

		return block;
	}

	auto ExpressionLetBlock::simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode*
	{
		auto* node = context.make_node<simulate::SimulateLetBlockNode>();

		// allocate variable space
		simulate::SimulateLetBlockNode::expressions_type expressions{
				reinterpret_cast<simulate::SimulateLetBlockNode::expressions_type::pointer>(context.allocate(sizeof(simulate::SimulateNode*) * initialized_variable_)),
				initialized_variable_};

		simulate::SimulateLetBlockNode::expressions_type::size_type index = 0;
		auto initialized_variables =
				variables_ |
				std::views::filter([](const auto& variable) { return variable->has_default_value(); });
		for (
			const auto& variable: initialized_variables)
		{
			auto* copy = [&context](const variable_type& v) -> simulate::SimulateNode*
			{
				const auto type = v->get_default_value()->result_type();
				auto* init = v->get_default_value()->simulate(context);
				auto* get = context.make_node<simulate::SimulateLocalGetNode>(v->get_stack_index());
				const auto type_size = v->get_type()->size_with_dimensions();

				if (type->is_xvalue())
				{
					return context.make_node<simulate::SimulateXValueCopyNode>(
							get,
							init,
							type_size);
				}

				switch (type->type())
				{
						using enum TypeDeclaration::variable_type;
					case BOOLEAN: { return context.make_node<simulate::SimulateLValueCopyNode<ExpressionConstantBoolean::value_type>>(get, init); }
					case INT: { return context.make_node<simulate::SimulateLValueCopyNode<ExpressionConstantInt::value_type>>(get, init); }
					case FLOAT: { return context.make_node<simulate::SimulateLValueCopyNode<ExpressionConstantFloat::value_type>>(get, init); }
					default: { throw std::runtime_error{"unsupported type!"}; }
				}
			}(variable);

			expressions[index++] = copy;
		}

		node->set_expression(expression_->simulate(context));
		return node;
	}

	auto ExpressionLetBlock::infer_type(infer_type_context& context) -> void
	{
		const auto prev_stack_top = context.stack_top;
		const auto prev_local_size = context.local_variables.size();
		initialized_variable_ = 0;

		for (auto& variable: variables_)
		{
			context.local_variables.push_back(variable);
			if (variable->has_default_value())
			{
				variable->get_default_value()->infer_type(context);
				if (!variable->get_type()->is_compatible_type(*variable->get_default_value()->result_type(), false)) { throw semantic_error{"variable initialization type mismatch!"}; }

				// todo: dereference default value
				++initialized_variable_;
			}

			variable->set_stack_index(context.stack_top);
			// todo: align to 16?
			context.stack_top += (variable->get_type()->size_with_dimensions() + 0xf) & ~0xf;
		}
		expression_->infer_type(context);

		context.function->set_stack_size(
				std::ranges::max(
						context.function->get_stack_size(),
						context.stack_top));
		context.stack_top = prev_stack_top;
		context.local_variables.resize(prev_local_size);
	}

	auto ExpressionLetBlock::deep_copy_from_this(expression_type dest) const -> expression_type
	{
		auto assignment = common_deep_copy_from_this<ExpressionLetBlock>(dest);

		// todo: copy variable
		assignment->variables_ = variables_;
		assignment->expression_ = expression_->deep_copy_from_this();
		return assignment;
	}

	auto ExpressionLetBlock::deep_copy_from_this() const -> expression_type
	{
		auto assignment = common_deep_copy_from_this<ExpressionLetBlock>(nullptr);

		// todo: copy variable
		assignment->variables_ = variables_;
		assignment->expression_ = expression_->deep_copy_from_this();
		return assignment;
	}

	auto ExpressionVariableDeclaration::simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode*
	{
		if (category_.is_local()) { return context.make_node<simulate::SimulateLocalGetNode>(variable_->get_stack_index()); }

		if (category_.is_global()) { return context.make_node<simulate::SimulateGlobalGetNode>(variable_->get_global_index()); }

		gsl_assert(category_.is_this_call_argument());
		return context.make_node<simulate::SimulateThisCallArgumentGet>(category_.this_call_argument_index());
	}

	auto ExpressionVariableDeclaration::infer_type(infer_type_context& context) -> void
	{
		// local variable on the stack
		if (const auto it = std::ranges::find(
					context.local_variables,
					name_,
					[](const auto& local) { return local->get_name(); }
					);
			it != context.local_variables.end())
		{
			category_.set_local();

			variable_ = *it;
			result_type_ = make<TypeDeclaration>(*(*it)->get_type());
			result_type_->set_xvalue(true);
			return;
		}

		// function argument
		const auto& arguments = context.function->get_arguments();
		for (decltype(arguments.size()) i = 0; i < arguments.size(); ++i)
		{
			if (arguments[i].get_name() == name_)
			{
				category_.bits = {i};

				variable_ = context.function->get_argument(i);
				result_type_ = make<TypeDeclaration>(*context.function->get_argument(i)->get_type());
				return;
			}
		}

		// global variable
		if (const auto g = context.mod->get_global(name_);
			!g)
		{
			throw semantic_error{
					string::format("cannot locate global variable '{}'",
					               name_)};
		}
		else
		{
			category_.set_global();

			variable_ = g;
			result_type_ = make<TypeDeclaration>(*g->get_type());
			result_type_->set_xvalue(true);
		}
	}

	auto ExpressionVariableDeclaration::deep_copy_from_this(expression_type dest) const -> expression_type
	{
		auto var = common_deep_copy_from_this<ExpressionVariableDeclaration>(dest);

		var->name_ = name_;
		// todo: lookup again?
		var->variable_ = variable_;
		var->category_ = category_;

		return var;
	}

	auto ExpressionVariableDeclaration::deep_copy_from_this() const -> expression_type
	{
		auto var = common_deep_copy_from_this<ExpressionVariableDeclaration>(nullptr);

		var->name_ = name_;
		// todo: lookup again?
		var->variable_ = variable_;
		var->category_ = category_;

		return var;
	}

	auto ExpressionFunctionCall::simulate(simulate::SimulateContext& context) const -> simulate::SimulateNode*
	{
		auto* node = dynamic_cast<simulate::SimulateFunctionCallNode*>(function_->make_simulate_node(context));

		gsl_assert(node);
		if (arguments_.empty()) { node->build(function_->get_function_index(), {}, {}); }
		else
		{
			// allocate pre_arguments space
			simulate::SimulateFunctionCallNode::pre_eval_arguments_type pre_arguments{
					reinterpret_cast<simulate::SimulateFunctionCallNode::pre_eval_arguments_type::pointer>(context.allocate(sizeof(simulate::SimulateNode*) * arguments_.size())),
					arguments_.size()};
			// set pre_arguments
			for (decltype(arguments_.size()) i = 0; i < arguments_.size(); ++i) { pre_arguments[i] = arguments_[i]->simulate(context); }

			// allocate post_arguments space for 'eval'
			const simulate::SimulateFunctionCallNode::post_eval_arguments_type post_arguments{
					reinterpret_cast<simulate::SimulateFunctionCallNode::post_eval_arguments_type::pointer>(context.allocate(sizeof(simulate::SimulateFunctionCallNode::value_type) * arguments_.size())),
					arguments_.size()};

			// build it
			node->build(function_->get_function_index(), pre_arguments, post_arguments);
		}

		return node;
	}

	auto ExpressionFunctionCall::infer_type(infer_type_context& context) -> void
	{
		// passed arguments type
		container::vector<type_declaration_type> arguments_type{};
		arguments_type.reserve(arguments_.size());
		for (const auto& argument: arguments_)
		{
			argument->infer_type(context);
			arguments_type.push_back(argument->result_type());
		}
		// find functions
		const auto functions = context.mod->get_matched_functions(name_, arguments_type);

		if (functions.empty())
		{
			throw semantic_error{
					string::format(
							"no matching function '{}'",
							name_)};
		}

		if (functions.size() != 1)
		{
			throw semantic_error{
					string::format(
							"too many matching function '{}'",
							name_)};
		}

		function_ = functions.front();
		// copy it
		gsl_assert(!has_result_type(), "result type overwrote!");
		result_type_ = make<TypeDeclaration>(*function_->get_return_type());

		// set default arguments
		const auto& function_arguments = function_->get_arguments();
		for (decltype(arguments_.size()) i = arguments_.size(); i < function_arguments.size(); ++i)
		{
			gsl_assert(function_arguments[i].has_default_value());
			auto extra_argument = function_arguments[i].get_default_value()->deep_copy_from_this();
			if (!extra_argument->has_result_type()) { extra_argument->infer_type(context); }
			arguments_.push_back(extra_argument);
		}

		for (decltype(arguments_.size()) i = 0; i < arguments_.size(); ++i)
		{
			if (!function_arguments[i].get_type()->is_xvalue())
			{
				// todo: reference
			}
		}
	}

	auto ExpressionFunctionCall::deep_copy_from_this(expression_type dest) const -> expression_type
	{
		gsl_assert(dest.operator bool());

		auto call = common_deep_copy_from_this<ExpressionFunctionCall>(dest);
		call->name_ = name_;

		// copy arguments
		call->arguments_.reserve(arguments_.size());
		for (auto& argument: arguments_) { call->arguments_.push_back(argument->deep_copy_from_this()); }

		call->function_ = function_;

		return call;
	}

	auto ExpressionFunctionCall::deep_copy_from_this() const -> expression_type
	{
		auto call = common_deep_copy_from_this<ExpressionFunctionCall>(nullptr);
		call->name_ = name_;

		// copy arguments
		call->arguments_.reserve(arguments_.size());
		for (auto& argument: arguments_) { call->arguments_.push_back(argument->deep_copy_from_this()); }

		call->function_ = function_;

		return call;
	}

	auto ExpressionOperator::deep_copy_from_this(const expression_type dest) const -> expression_type
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

	auto ExpressionUnaryOperator::infer_type(infer_type_context& context) -> void
	{
		rhs_->infer_type(context);

		// passed arguments type
		auto rhs_result_type = rhs_->result_type();
		const std::span arguments_type{&rhs_result_type, 1};
		// find functions
		const auto functions = context.mod->get_matched_functions(
				// todo: operand name?
				magic_enum::enum_name(operand_),
				arguments_type);

		if (functions.empty())
		{
			throw semantic_error{
					string::format(
							"no matching function '{}' {}",
							magic_enum::enum_name(operand_),
							*rhs_->result_type())};
		}

		if (functions.size() != 1)
		{
			throw semantic_error{
					string::format(
							"too many matching function '{}' {}",
							magic_enum::enum_name(operand_),
							*rhs_->result_type())};
		}

		// todo
		if (!functions.front()->is_builtin())
		{
			throw semantic_error{
					string::format(
							"operator must point to a builtin function for now")};
		}

		function_ = functions.front();
		gsl_assert(function_->get_arguments().size() == 1);
		// copy it
		gsl_assert(!has_result_type(), "result type overwrote!");
		result_type_ = make<TypeDeclaration>(*function_->get_return_type());

		// todo: dereference
		if (!function_->get_arguments()[0].get_type()->is_xvalue()) { }
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

	auto ExpressionBinaryOperator::infer_type(infer_type_context& context) -> void
	{
		lhs_->infer_type(context);
		rhs_->infer_type(context);

		// passed arguments type
		container::vector<type_declaration_type> arguments_type{lhs_->result_type(), rhs_->result_type()};
		// find functions
		const auto functions = context.mod->get_matched_functions(
				// todo: operand name?
				magic_enum::enum_name(operand_),
				arguments_type);

		if (functions.empty())
		{
			throw semantic_error{
					string::format(
							"no matching function '{}' {} {}",
							magic_enum::enum_name(operand_),
							*lhs_->result_type(),
							*rhs_->result_type())};
		}

		if (functions.size() != 1)
		{
			throw semantic_error{
					string::format(
							"too many matching function '{}' {} {}",
							magic_enum::enum_name(operand_),
							*lhs_->result_type(),
							*rhs_->result_type())};
		}

		// todo
		if (!functions.front()->is_builtin())
		{
			throw semantic_error{
					string::format(
							"operator must point to a builtin function for now")};
		}

		function_ = functions.front();
		gsl_assert(function_->get_arguments().size() == 2);
		// copy it
		gsl_assert(!has_result_type(), "result type overwrote!");
		result_type_ = make<TypeDeclaration>(*function_->get_return_type());

		// todo: dereference
		if (!function_->get_arguments()[0].get_type()->is_xvalue()) { }
		if (!function_->get_arguments()[1].get_type()->is_xvalue()) { }
	}

	auto ExpressionBinaryOperator::deep_copy_from_this(const expression_type dest) const -> expression_type
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

	auto ExpressionConstantInt::infer_type(infer_type_context& context) -> void
	{
		gsl_assert(!has_result_type(), "result type overwrote!");
		(void)context;
		result_type_ = make<TypeDeclaration>(TypeDeclaration::variable_type::INT);
	}

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

	auto ExpressionConstantFloat::infer_type(infer_type_context& context) -> void
	{
		gsl_assert(!has_result_type(), "result type overwrote!");
		(void)context;
		result_type_ = make<TypeDeclaration>(TypeDeclaration::variable_type::FLOAT);
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

	auto ExpressionConstantString::infer_type(infer_type_context& context) -> void
	{
		gsl_assert(!has_result_type(), "result type overwrote!");
		(void)context;
		result_type_ = make<TypeDeclaration>(TypeDeclaration::variable_type::STRING);
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

	auto Module::get_matched_functions(const symbol_name_view name, std::span<type_declaration_type> arguments_type) -> container::vector<function_type>
	{
		container::vector<function_type> result{};
		auto functions =
				functions_ |
				std::views::filter(
						[name](const auto& pair) { return pair.first == name; }) |
				std::views::values;
		for (
			const auto& function: functions)
		{
			if (const auto required_arguments = function->get_arguments();
				required_arguments.size() >= arguments_type.size())
			{
				// check whether the missing parameter has a default value.
				bool tail_compatible = true;
				for (const auto tail_arguments = required_arguments |
				                                 std::views::reverse |
				                                 std::views::take(required_arguments.size() - arguments_type.size());
				     const auto& argument: tail_arguments)
				{
					if (!argument.has_default_value())
					{
						tail_compatible = false;
						break;
					}
				}
				if (!tail_compatible)
				{
					// not compatible, ignore it
					continue;
				}

				// check whether the parameter types passed in are compatible
				bool types_compatible = true;
				utility::zip_invoke(
						arguments_type.begin(),
						arguments_type.end(),
						[&types_compatible](const auto& passed_type, const auto& required_variable) -> bool
						{
							if (const auto& required_type = required_variable.get_type();
								(required_type->is_xvalue() && !passed_type->is_xvalue()) ||
								(!required_type->is_compatible_type(*passed_type, false)))
							{
								types_compatible = false;
								return false;
							}
							return true;
						},
						required_arguments.begin()
						);

				if (types_compatible) { result.push_back(function); }
			}
		}

		return result;
	}
}
