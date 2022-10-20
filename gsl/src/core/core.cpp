#include <gsl/core/core.hpp>

namespace gal::gsl::core
{
	ModuleNode::~ModuleNode() = default;

	auto ModuleNode::visit(ModuleVisitor& visitor) -> ModuleNode*
	{
		visitor.before_visit(this);
		// unknown operator
		visitor.visit_operator("??");
		return visitor.after_visit(this);
	}

	ModuleVisitor::~ModuleVisitor() = default;
}
