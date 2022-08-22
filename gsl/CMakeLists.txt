project(
		gal-script-lang
		LANGUAGES CXX
)

set(
	${PROJECT_NAME}_SOURCE

	src/runtime_info.cpp
)

add_library(
	${PROJECT_NAME}
	# todo
	STATIC

	${${PROJECT_NAME}_SOURCE}
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC 
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(
		gal::GSL
		ALIAS
		${PROJECT_NAME}
)

target_compile_definitions(
			${PROJECT_NAME}
			INTERFACE 
			${PROJECT_NAME_PREFIX}MAJOR_VERSION=${${PROJECT_NAME_PREFIX}MAJOR_VERSION}
			${PROJECT_NAME_PREFIX}MINOR_VERSION=${${PROJECT_NAME_PREFIX}MINOR_VERSION}
			${PROJECT_NAME_PREFIX}PATCH_VERSION=${${PROJECT_NAME_PREFIX}PATCH_VERSION}
			${PROJECT_NAME_PREFIX}VERSION="${${PROJECT_NAME_PREFIX}VERSION}"
			${PROJECT_NAME_PREFIX}COMPILER_NAME="${${PROJECT_NAME_PREFIX}COMPILER_NAME}"
			${PROJECT_NAME_PREFIX}COMPILER_VERSION="${${PROJECT_NAME_PREFIX}COMPILER_VERSION}"
			${PROJECT_NAME_PREFIX}COMPILER_HAS_META=${${PROJECT_NAME_PREFIX}COMPILER_HAS_META}

			$<$<CXX_COMPILER_ID:MSVC>:${PROJECT_NAME_PREFIX}COMPILER_MSVC>
			$<$<CXX_COMPILER_ID:GNU>:${PROJECT_NAME_PREFIX}COMPILER_GNU>
			$<$<CXX_COMPILER_ID:Clang>:${PROJECT_NAME_PREFIX}COMPILER_CLANG>
			# TODO: MORE COMPILERS HERE.
)

set(CMAKE_CXX_STANDARD 23)
set_compile_options_private(${PROJECT_NAME})

include(${${PROJECT_NAME_PREFIX}CMAKE_3RDPARTY_PATH}/eve.cmake)
include(${${PROJECT_NAME_PREFIX}CMAKE_3RDPARTY_PATH}/foonathan-memory.cmake)
include(${${PROJECT_NAME_PREFIX}CMAKE_3RDPARTY_PATH}/spdlog.cmake)

target_link_libraries(
		${PROJECT_NAME}
		PUBLIC
		eve::eve
		foonathan_memory
		# spdlog
		spdlog_header_only
)

include(CheckCXXSourceCompiles)
CHECK_CXX_SOURCE_COMPILES(
	"
	#include <format>
	int main()
	{
		std::cout << std::format(\"hello {}!\", \"world\");
	}
	"
	COMPILER_SUPPORT_FORMAT
)

if (${COMPILER_SUPPORT_FORMAT})
	message("${PROJECT_NAME} info: current compiler ${CMAKE_CXX_COMPILER} support std::format.")
else ()
	message("${PROJECT_NAME} info: current compiler ${CMAKE_CXX_COMPILER} not support std::format, use fmtlib instead now.")
	target_compile_definitions(
			${PROJECT_NAME}
			PUBLIC
			GSL_FMT_NOT_SUPPORT
			SPDLOG_FMT_EXTERNAL
	)
	include(${${PROJECT_NAME_PREFIX}CMAKE_3RDPARTY_PATH}/fmtlib.cmake)
	message("${PROJECT_NAME} info: build fmtlib as a public module of ${PROJECT_NAME}.")
	target_link_libraries(
			${PROJECT_NAME}
			PUBLIC
			fmt
	)
endif (${COMPILER_SUPPORT_FORMAT})
