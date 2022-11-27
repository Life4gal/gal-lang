#pragma once

#include <gsl/string/string.hpp>
#include <gsl/string/string_view.hpp>
#include <gsl/container/vector.hpp>
#include <gsl/type/value.hpp>
#include <gsl/memory/allocator.hpp>
#include <gsl/debug/assert.hpp>

#include <span>
#include <source_location>

namespace gal::gsl::simulate
{
	class SimulateNode;
	class SimulateContext;

	class SimulateNode
	{
	public:
		using value_type = type::BoxedValue;

	protected:
	public:
		SimulateNode() = default;
		SimulateNode(const SimulateNode& other) = default;
		SimulateNode(SimulateNode&& other) noexcept = default;
		auto operator=(const SimulateNode& other) -> SimulateNode& = default;
		auto operator=(SimulateNode&& other) noexcept -> SimulateNode& = default;

		virtual ~SimulateNode() noexcept;

		virtual auto eval(SimulateContext& context) -> value_type = 0;
	};

	class SimulateGlobalVariable final
	{
	public:
		using name_type = string::string::pointer;
		using size_type = string::string::size_type;
		using value_type = SimulateNode::value_type;

	private:
		name_type name_;
		size_type size_;
		value_type value_;

	public:
		// todo: ctor

		[[nodiscard]] constexpr auto name() const noexcept -> name_type { return name_; }

		[[nodiscard]] constexpr auto value() const noexcept -> value_type { return value_; }
	};

	class SimulateFunction final
	{
	public:
		using name_type = string::string::pointer;
		using size_type = std::ptrdiff_t;

	private:
		name_type name_;
		SimulateNode* code_;
		size_type stack_size_;

	public:
		[[nodiscard]] constexpr auto stack_size() const noexcept -> size_type { return stack_size_; }

		auto eval(SimulateContext& context) const -> SimulateNode::value_type { return code_->eval(context); }
	};

	class SimulateContext
	{
	public:
		using any_allocator_type = memory::AnyAllocator<char>;
		using any_allocator_trait_type = std::allocator_traits<any_allocator_type>;

		using data_type = any_allocator_type::pointer;

		using global_variable_container_type = std::span<SimulateGlobalVariable>;
		using global_variable_index_type = global_variable_container_type::size_type;

		using function_container_type = std::span<SimulateFunction>;
		using function_index_type = function_container_type::size_type;

		constexpr static function_index_type invalid_function_index{static_cast<function_index_type>(-1)};

		constexpr static std::size_t chunk_size{1024 * 1024};
		using chunk_type = std::span<any_allocator_type::value_type, chunk_size>;

		constexpr static std::size_t stack_size{1024 * 16};
		using stack_type = std::span<any_allocator_type::value_type, stack_size>;

	private:
		[[no_unique_address]] any_allocator_type any_allocator_;

		global_variable_container_type global_variables_;
		function_container_type functions_;
		SimulateNode::value_type* function_arguments_;

		chunk_type chunk_;
		chunk_type::size_type current_chunk_used_;
		stack_type stack_;
		stack_type::size_type current_stack_used_;

	public:
		SimulateContext();

		SimulateContext(const SimulateContext& other) = delete;
		SimulateContext(SimulateContext&& other) noexcept = delete;
		auto operator=(const SimulateContext& other) -> SimulateContext& = delete;
		auto operator=(SimulateContext&& other) noexcept -> SimulateContext& = delete;

		~SimulateContext() noexcept;

		[[nodiscard]] auto allocate(chunk_type::size_type size) -> chunk_type::pointer;

		template<typename NodeType, typename... Args>
		[[nodiscard]] auto make_node(Args&&... args) -> NodeType* { return new(allocate(sizeof(NodeType))) NodeType{std::forward<Args>(args)...}; }

		[[noreturn]] void throw_error(string::string_view message, const std::source_location& location = std::source_location::current());

		auto call_function(function_index_type index, SimulateNode::value_type* args) -> SimulateNode::value_type;

		[[nodiscard]] constexpr auto get_global_variable(const global_variable_index_type index) const -> const SimulateGlobalVariable& { return global_variables_[index]; }

		[[nodiscard]] constexpr auto get_function_argument(const function_index_type index) const -> SimulateNode::value_type { return function_arguments_[index]; }

		[[nodiscard]] constexpr auto get_stack_value(const stack_type::size_type index) const noexcept -> SimulateNode::value_type { return type::BoxedCaster::from(stack_.data() + current_stack_used_ + index); }
	};

	class SimulateFieldNode final : public SimulateNode
	{
	public:
		using offset_type = std::ptrdiff_t;

	private:
		SimulateNode* object_;
		offset_type offset_;

	public:
		SimulateFieldNode(SimulateNode* object, const offset_type offset)
			: object_{object},
			  offset_{offset} {}

		[[nodiscard]] auto eval(SimulateContext& context) -> value_type override
		{
			auto obj = object_->eval(context);
			const auto address = type::BoxedCaster::to<char*>(obj);
			return type::BoxedCaster::from(address + offset_);
		}
	};

	class SimulateLocalGetNode final : public SimulateNode
	{
	public:
		using size_type = SimulateContext::stack_type::size_type;

	private:
		size_type index_;

	public:
		explicit SimulateLocalGetNode(const size_type index)
			: index_{index} {}

		auto eval(SimulateContext& context) -> value_type override { return context.get_stack_value(index_); }
	};

	class SimulateGlobalGetNode final : public SimulateNode
	{
	public:
		using size_type = SimulateContext::global_variable_index_type;

	private:
		size_type index_;

	public:
		explicit SimulateGlobalGetNode(const size_type index)
			: index_{index} {}

		auto eval(SimulateContext& context) -> value_type override { return context.get_global_variable(index_).value(); }
	};

	class SimulateThisCallArgumentGet final : public SimulateNode
	{
	public:
		using size_type = SimulateContext::function_index_type;

	private:
		size_type argument_index_;

	public:
		explicit SimulateThisCallArgumentGet(const size_type index)
			: argument_index_{index} {}

		void set_index(const size_type index) noexcept { argument_index_ = index; }

		auto eval(SimulateContext& context) -> value_type override { return context.get_function_argument(argument_index_); }
	};

	class SimulateBlockNode : public SimulateNode
	{
	public:
		using expressions_type = std::span<SimulateNode*>;

	protected:
		expressions_type expressions_;

	public:
		void set_expressions(const expressions_type expressions) { expressions_ = expressions; }

		auto eval(SimulateContext& context) -> value_type override
		{
			for (const auto expression: expressions_) { expression->eval(context); }
			return {};
		}
	};

	class SimulateLetBlockNode final : public SimulateBlockNode
	{
	public:
	private:
		SimulateNode* expression_{nullptr};

	public:
		void set_expression(SimulateNode* expression) noexcept { expression_ = expression; }

		auto eval(SimulateContext& context) -> value_type override
		{
			gsl_assert(expression_);

			SimulateBlockNode::eval(context);
			return expression_->eval(context);
		}
	};

	class SimulateFunctionCallNode final : public SimulateNode
	{
	public:
		using index_type = SimulateContext::function_index_type;

		using pre_eval_arguments_type = std::span<SimulateNode*>;
		using post_eval_arguments_type = std::span<value_type>;
		using argument_index_type = pre_eval_arguments_type::size_type;

	private:
		index_type function_index_;
		pre_eval_arguments_type pre_arguments_;
		post_eval_arguments_type post_arguments_;

	public:
		// construct a empty call
		SimulateFunctionCallNode()
			: function_index_{SimulateContext::invalid_function_index} {}

		void build(
				const index_type index,
				const pre_eval_arguments_type pre_arguments,
				const post_eval_arguments_type post_arguments)
		{
			function_index_ = index;
			pre_arguments_ = pre_arguments;
			post_arguments_ = post_arguments;
		}

		auto eval(SimulateContext& context) -> value_type override
		{
			for (argument_index_type i = 0; i < pre_arguments_.size(); ++i) { post_arguments_[i] = pre_arguments_[i]->eval(context); }

			return context.call_function(function_index_, post_arguments_.data());
		}
	};

	class SimulateUnaryOperatorNodeBase : public SimulateNode
	{
	protected:
		SimulateNode* rhs_;

	public:
		void set_simulate_node(SimulateNode* node) { rhs_ = node; }
	};

	template<typename SimulatePolicy>
	class SimulateUnaryNegateNode final : public SimulateUnaryOperatorNodeBase
	{
		auto eval(SimulateContext& context) -> value_type override;
	};

	template<typename SimulatePolicy>
	class SimulateUnaryBitComplementNode final : public SimulateUnaryOperatorNodeBase
	{
		auto eval(SimulateContext& context) -> value_type override;
	};

	template<typename SimulatePolicy>
	class SimulateUnaryLogicalNotNode final : public SimulateUnaryOperatorNodeBase
	{
		auto eval(SimulateContext& context) -> value_type override;
	};

	class SimulateBinaryOperatorNodeBase : public SimulateNode
	{
	protected:
		SimulateNode* lhs_;
		SimulateNode* rhs_;

	public:
		void set_lhs_simulate_node(SimulateNode* node) { lhs_ = node; }
		void set_rhs_simulate_node(SimulateNode* node) { rhs_ = node; }
	};

	template<typename ValueType>
	class SimulateConstantNode final : public SimulateNode
	{
	public:
		using value_type = ValueType;

	private:
		SimulateNode::value_type value_;

	public:
		explicit SimulateConstantNode(value_type&& value)
			: value_{type::BoxedCaster::from(std::move(value))} {}

		explicit SimulateConstantNode(const value_type& value)
			: value_{type::BoxedCaster::from(value)} {}

		auto eval(SimulateContext& context) -> SimulateNode::value_type override
		{
			(void)context;
			return value_;
		}
	};

	class SimulateXValueCopyNode final : public SimulateNode
	{
	public:
		using size_type = std::size_t;

	private:
		SimulateNode* lhs_;
		SimulateNode* rhs_;
		size_type size_;

	public:
		SimulateXValueCopyNode(
				SimulateNode* lhs,
				SimulateNode* rhs,
				const size_type size
				)
			: lhs_{lhs},
			  rhs_{rhs},
			  size_{size} {}

		auto eval(SimulateContext& context) -> value_type override
		{
			auto lhs = lhs_->eval(context);
			const auto rhs = rhs_->eval(context);

			// todo: plain copy?
			type::BoxedCaster::copy(lhs, rhs, size_);
			return lhs;
		}
	};

	template<typename ValueType>
	class SimulateLValueCopyNode final : public SimulateNode
	{
	public:
		using value_type = ValueType;

	private:
		SimulateNode* lhs_;
		SimulateNode* rhs_;

	public:
		SimulateLValueCopyNode(
				SimulateNode* lhs,
				SimulateNode* rhs)
			: lhs_{lhs},
			  rhs_{rhs} {}

		auto eval(SimulateContext& context) -> SimulateNode::value_type override
		{
			auto lhs = lhs_->eval(context);
			const auto rhs = rhs_->eval(context);

			type::BoxedCaster::copy<value_type>(lhs, rhs);
			return lhs;
		}
	};
}
