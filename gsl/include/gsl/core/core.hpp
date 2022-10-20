#pragma once

#include <gsl/core/file.hpp>
#include <gsl/core/value.hpp>
#include <gsl/accelerate/string_view.hpp>

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
}
