#pragma once

#include <gsl/string/string.hpp>
#include <gsl/string/string_view.hpp>
#include <fmt/format.h>

namespace gal::gsl::string
{
	template<typename... Args>
	[[nodiscard]] constexpr auto format(
			fmt::format_string<Args...> fmt,
			Args&&... args
			) -> string
	{
		string ret{};
		fmt::format_to(
				std::back_inserter(ret),
				fmt,
				std::forward<Args>(args)...);
		return ret;
	}

	template<typename OutputIt, typename... Args>
	[[nodiscard]] constexpr auto format_to(
			OutputIt out,
			fmt::format_string<Args...> fmt,
			Args&&... args
			) -> OutputIt { return fmt::format_to(out, fmt, std::forward<Args>(args)...); }

	[[nodiscard]] auto vformat(
			const fmt::string_view fmt,
			fmt::format_args args
			)
	{
		string ret{};
		fmt::vformat_to(
				std::back_inserter(ret),
				fmt,
				args);
		return ret;
	}

	template<typename OutputIt>
	[[nodiscard]] auto vformat_to(
			OutputIt out,
			const fmt::string_view fmt,
			fmt::format_args args) -> OutputIt { return fmt::vformat_to(out, fmt, args); }
}

namespace fmt
{
	namespace string = gal::gsl::string;

	template<>
	struct formatter<string::string_view>
	{
		template<typename ParseContext>
		constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const string::string_view& string, FormatContext& ctx)
		{
			return string::format_to(
					ctx.out(),
					"{}",
					string);
		}
	};

	template<>
	struct formatter<string::string> : public formatter<string::string_view>
	{
		template<typename ParseContext>
		constexpr auto parse(ParseContext& ctx) { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const string::string& string, FormatContext& ctx) { return formatter<string::string_view>::format(string, ctx); }
	};
}// namespace fmt
