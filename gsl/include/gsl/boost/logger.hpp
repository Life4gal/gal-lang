#pragma once

#ifndef GSL_NO_SPDLOG
#include <spdlog/spdlog.h>
#define GSL_LOGGER_DO(...) __VA_ARGS__
#else
	#define GSL_LOGGER_DO(...)
#endif


namespace gal::gsl::boost::logger
{
	namespace logger_detail
	{
		#ifndef GSL_NO_SPDLOG
		template<typename... Args>
		using format_string_t = spdlog::format_string_t<Args...>;
		#else
			template<typename... Args>
			struct format_string_t {};
		#endif
	}

	template<typename... Args>
	void trace([[maybe_unused]] logger_detail::format_string_t<Args...> fmt, [[maybe_unused]] Args&&... args)
	{
		GSL_LOGGER_DO(spdlog::trace(fmt, std::forward<Args>(args)...);)
	}

	template<typename... Args>
	void debug([[maybe_unused]] logger_detail::format_string_t<Args...> fmt, [[maybe_unused]] Args&&... args)
	{
		GSL_LOGGER_DO(spdlog::debug(fmt, std::forward<Args>(args)...));
	}

	template<typename... Args>
	void info([[maybe_unused]] logger_detail::format_string_t<Args...> fmt, [[maybe_unused]] Args&&... args)
	{
		GSL_LOGGER_DO(spdlog::info(fmt, std::forward<Args>(args)...));
	}

	template<typename... Args>
	void warn([[maybe_unused]] logger_detail::format_string_t<Args...> fmt, [[maybe_unused]] Args&&... args)
	{
		GSL_LOGGER_DO(spdlog::warn(fmt, std::forward<Args>(args)...));
	}

	template<typename... Args>
	void error([[maybe_unused]] logger_detail::format_string_t<Args...> fmt, [[maybe_unused]] Args&&... args)
	{
		GSL_LOGGER_DO(spdlog::error(fmt, std::forward<Args>(args)...));
	}

	template<typename... Args>
	void critical([[maybe_unused]] logger_detail::format_string_t<Args...> fmt, [[maybe_unused]] Args&&... args)
	{
		GSL_LOGGER_DO(spdlog::critical(fmt, std::forward<Args>(args)...));
	}

	// constexpr int logger_fatal_exit_code = -42;

	template<typename... Args>
	void fatal([[maybe_unused]] logger_detail::format_string_t<Args...> fmt, [[maybe_unused]] Args&&... args)
	{
		critical(fmt, std::forward<Args>(args)...);
		// std::exit(logger_fatal_exit_code);
		throw std::runtime_error{"Fatal Error."};
	}
}
