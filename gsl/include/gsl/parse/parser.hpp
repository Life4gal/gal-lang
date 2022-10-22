#pragma once

#include <gsl/string/string.hpp>
#include <gsl/string/string_view.hpp>
#include <gsl/container/vector.hpp>
#include <optional>
#include <variant>

namespace gal::gsl::parse
{
	struct node_bool
	{
		bool value;

		template<typename Function>
		auto invoke(Function&& function) const -> void { std::invoke(std::forward<Function>(function), value); }
	};

	// The actual type will be selected according to the length
	struct node_number
	{
		using value_type = string::string;
		// using value_type = std::uint64_t;
		using fraction_type = string::string;
		using exponent_type = std::int16_t;

		value_type value;
		std::optional<fraction_type> fraction;
		std::optional<std::int16_t> exponent;

		template<typename Function>
		auto invoke(Function&& function) const -> void
		{
			std::invoke(std::forward<Function>(function), value);
			if (fraction.has_value()) { std::invoke(std::forward<Function>(function), *fraction); }

			if (exponent.has_value()) { std::invoke(std::forward<Function>(function), *exponent); }
		}
	};

	struct node_string
	{
		using value_type = string::string;

		value_type value;

		template<typename Function>
		auto invoke(Function&& function) const -> void { std::invoke(std::forward<Function>(function), value); }
	};

	class Node
	{
	public:
		using value_type = std::variant<
			node_bool,
			node_number,
			node_string>;

	private:
		value_type value_;

	public:
		// todo: allow invalid state?
		Node() = delete;

		template<typename T>
			requires std::is_constructible_v<value_type, T>
		explicit(false) Node(T&& arg)
			: value_{std::forward<T>(arg)} {}

		// test
		template<typename Function>
		auto invoke(Function&& function) const -> void { std::visit([f = std::forward<Function>(function)](const auto& value) mutable { value.invoke(std::forward<Function>(f)); }, value_); }
	};

	using node_tree_result = container::vector<Node>;

	[[nodiscard]] auto parse_file(string::string_view filename) -> node_tree_result;
}
