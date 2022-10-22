#pragma once

#ifndef GSL_NO_LOGGER
#include <spdlog/spdlog.h>
#endif

namespace gal::gsl::logger
{
	namespace logger_detail
	{
		template<typename... Args>
		#ifdef GSL_NO_LOGGER
		struct format_string_t {};
		#else
		using format_string_t = spdlog::format_string_t<Args...>;
		#endif
	}

	template<typename... Args>
	auto trace([[maybe_unused]] logger_detail::format_string_t<Args...> fmt, [[maybe_unused]] Args&&... args) -> void
	{
		#ifndef GSL_NO_LOGGER
		spdlog::trace(fmt, std::forward<Args>(args)...);
		#endif
	}

	template<typename... Args>
	auto debug([[maybe_unused]] logger_detail::format_string_t<Args...> fmt, [[maybe_unused]] Args&&... args) -> void
	{
		#ifndef GSL_NO_LOGGER
		spdlog::debug(fmt, std::forward<Args>(args)...);
		#endif
	}

	template<typename... Args>
	auto info([[maybe_unused]] logger_detail::format_string_t<Args...> fmt, [[maybe_unused]] Args&&... args) -> void
	{
		#ifndef GSL_NO_LOGGER
		spdlog::info(fmt, std::forward<Args>(args)...);
		#endif
	}

	template<typename... Args>
	auto warn([[maybe_unused]] logger_detail::format_string_t<Args...> fmt, [[maybe_unused]] Args&&... args) -> void
	{
		#ifndef GSL_NO_LOGGER
		spdlog::warn(fmt, std::forward<Args>(args)...);
		#endif
	}

	template<typename... Args>
	auto error([[maybe_unused]] logger_detail::format_string_t<Args...> fmt, [[maybe_unused]] Args&&... args) -> void
	{
		#ifndef GSL_NO_LOGGER
		spdlog::error(fmt, std::forward<Args>(args)...);
		#endif
	}

	template<typename... Args>
	auto critical([[maybe_unused]] logger_detail::format_string_t<Args...> fmt, [[maybe_unused]] Args&&... args) -> void
	{
		#ifndef GSL_NO_LOGGER
		spdlog::critical(fmt, std::forward<Args>(args)...);
		#endif
	}
}
