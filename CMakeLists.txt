cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME_PREFIX "GAL_SCRIPT_LANG_")

set(
		${PROJECT_NAME_PREFIX}MAJOR_VERSION
		0
)

set(
		${PROJECT_NAME_PREFIX}MINOR_VERSION
		0
)

set(
		${PROJECT_NAME_PREFIX}PATCH_VERSION
		1
)

set(${PROJECT_NAME_PREFIX}VERSION ${${PROJECT_NAME_PREFIX}MAJOR_VERSION}.${${PROJECT_NAME_PREFIX}MINOR_VERSION}.${${PROJECT_NAME_PREFIX}PATCH_VERSION})

project(
		gal-script-lang
		VERSION ${${PROJECT_NAME_PREFIX}VERSION}
		DESCRIPTION "gal script language"
		HOMEPAGE_URL "https://github.com/Life4gal/gal-script-lang"
		LANGUAGES CXX
)

set(${PROJECT_NAME_PREFIX}COMPILER_NAME ${CMAKE_CXX_COMPILER_ID})
set(${PROJECT_NAME_PREFIX}COMPILER_VERSION ${${PROJECT_NAME_PREFIX}COMPILER_NAME}.${CMAKE_CXX_COMPILER_VERSION})

set(${PROJECT_NAME_PREFIX}GLOBAL_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(${PROJECT_NAME_PREFIX}CMAKE_CONFIG_PATH ${${PROJECT_NAME_PREFIX}GLOBAL_SOURCE_PATH}/config)
set(${PROJECT_NAME_PREFIX}CMAKE_MODULE_PATH ${${PROJECT_NAME_PREFIX}GLOBAL_SOURCE_PATH}/cmake)
set(${PROJECT_NAME_PREFIX}CMAKE_3RDPARTY_PATH ${${PROJECT_NAME_PREFIX}GLOBAL_SOURCE_PATH}/3rd-party)

# todo: Additional compiler options for different build types.
macro(set_compile_options_private target_name)
	message("Setting compile parameters for project ${target_name}...")
	target_compile_options(
		${target_name}
		PRIVATE

		-Wall -Wextra --pedantic -Werror -std=c++20
	)
endmacro()

macro(set_compile_options_public target_name)
	message("Setting compile parameters for project ${target_name}...")
	target_compile_options(
		${target_name}
		PUBLIC

		-Wall -Wextra --pedantic -Werror -std=c++20
	)
endmacro()

macro(set_compile_options_interface target_name)
	message("Setting compile parameters for project ${target_name}...")
	target_compile_options(
		${target_name}
		INTERFACE

		-Wall -Wextra --pedantic -Werror -std=c++20
	)
endmacro()

add_subdirectory(gsl)
enable_testing()
add_subdirectory(standalone_test)
add_subdirectory(unit_test)
