# Download and unpack eve at configure time
configure_file(
	${${PROJECT_NAME_PREFIX}CMAKE_CONFIG_PATH}/eve.in 
	eve-download/CMakeLists.txt
)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/eve-download)
if (result)
	message(FATAL_ERROR "CMake step for eve failed: ${result}")
endif ()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/eve-download)
if (result)
	message(FATAL_ERROR "Build step for eve failed: ${result}")
endif ()

# Add eve directly to our build.
add_subdirectory(
		${CMAKE_CURRENT_BINARY_DIR}/eve-src
		${CMAKE_CURRENT_BINARY_DIR}/eve-build
		EXCLUDE_FROM_ALL
)

set( EVE_USE_PCH             OFF )
set( EVE_BUILD_TEST          OFF )
set( EVE_BUILD_RANDOM        OFF )
set( EVE_BUILD_INTEGRATION   OFF )
set( EVE_BUILD_BENCHMARKS    OFF )
set( EVE_BUILD_DOCUMENTATION OFF )
