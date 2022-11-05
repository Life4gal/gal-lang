#include <gsl/backend/ast.hpp>
#include <gsl/debug/assert.hpp>

#include <magic_enum.hpp>

#include <algorithm>

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

	Function::~Function() noexcept = default;

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
