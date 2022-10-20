#pragma once

#include <gsl/core/file.hpp>
#include <gsl/core/value.hpp>
#include <gsl/accelerate/string_view.hpp>
#include <gsl/config.hpp>

namespace gal::gsl::core
{
	class ModuleNode;
	class ModuleVisitor;
	class ModuleContext;

	class ModuleNode
	{
	public:
	protected:
		LineInfo debug_info_;

		virtual ~ModuleNode() = 0;

	public:
		ModuleNode() = default;
		ModuleNode(const ModuleNode&) = default;
		auto operator=(const ModuleNode&) -> ModuleNode& = default;
		ModuleNode(ModuleNode&&) = default;
		auto operator=(ModuleNode&&) -> ModuleNode& = default;

		template<typename T>
		[[nodiscard]] auto eval(ModuleContext& context) -> decltype(auto) { return this->eval(context).as<T>(); }

		[[nodiscard]] virtual auto visit(ModuleVisitor& visitor) -> ModuleNode*;

	private:
		[[nodiscard]] virtual auto eval(ModuleContext& context) -> Value = 0;
	};

	class ModuleVisitor
	{
	public:
		using size_type = std::uint32_t;

		virtual ~ModuleVisitor();
		ModuleVisitor() = default;
		ModuleVisitor(const ModuleVisitor&) = default;
		auto operator=(const ModuleVisitor&) -> ModuleVisitor& = default;
		ModuleVisitor(ModuleVisitor&&) = default;
		auto operator=(ModuleVisitor&&) -> ModuleVisitor& = default;

		virtual auto before_visit(ModuleNode* node) -> void { (void)node; }

		virtual auto visit_operator(const char* name, const size_type size = 0, const accelerate::string_view type = {}) -> void
		{
			(void)name;
			(void)size;
			(void)type;
		}

		// todo: arguments

		virtual auto after_visit(ModuleNode* node) -> ModuleNode* { return node; }
	};

	struct stack_prologue { };

	class ModuleStackWalker : accelerate::enable_shared_from_this<ModuleStackWalker>
	{
	public:
		virtual ~ModuleStackWalker();
		ModuleStackWalker() = default;
		ModuleStackWalker(const ModuleStackWalker&) = default;
		auto operator=(const ModuleStackWalker&) -> ModuleStackWalker& = default;
		ModuleStackWalker(ModuleStackWalker&&) = default;
		auto operator=(ModuleStackWalker&&) -> ModuleStackWalker& = default;

		[[nodiscard]] virtual auto arguments_walkable() -> bool { return true; }
		[[nodiscard]] virtual auto variables_walkable() -> bool { return true; }
		[[nodiscard]] virtual auto out_of_scope_variables_walkable() -> bool { return true; }

		virtual auto walk_before_call(stack_prologue* prologue, const heap_data_type data) -> void
		{
			(void)prologue;
			(void)data;
		}

		// todo: details

		virtual auto walk_after_call(stack_prologue* prologue) -> bool
		{
			(void)prologue;
			return true;
		}
	};
}
