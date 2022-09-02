#pragma once

#include <fmt/core.h>

#include <gsl/accelerate/string.hpp>

// line_info
#include <gsl/core/core_detail/line_info.hpp>

template<>
struct fmt::formatter<gal::gsl::core::line_info, gal::gsl::accelerate::string::value_type>
{
	// ReSharper disable once CppMemberFunctionMayBeConst
	constexpr auto parse(basic_format_parse_context<gal::gsl::accelerate::string::value_type>& ctx) -> decltype(ctx.begin())
	{
		// nothing interesting here.
		(void)this;

		// ignore everything.
		auto it = ctx.begin();
		while (it != ctx.end() && *it != '}') { ++it; }

		return it;
	}

	template<typename FormatContext>
	constexpr auto format(const gal::gsl::core::line_info& line, FormatContext& ctx) const -> decltype(ctx.out())
	{
		if (line.info)
		{
			return format_to(
					ctx.out(),
					"[{}]({},{})-({},{})",
					line.info->get_filename(),
					line.line(),
					line.column(),
					line.last_line(),
					line.last_column());
		}

		return format_to(ctx.out(), "<unknown line info>");
	}
};
