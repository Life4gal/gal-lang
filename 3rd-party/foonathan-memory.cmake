# Download and unpack foonathan-memory at configure time
configure_file(
	${${PROJECT_NAME_PREFIX}CMAKE_CONFIG_PATH}/foonathan-memory.in 
	foonathan-memory-download/CMakeLists.txt
)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/foonathan-memory-download)
if (result)
	message(FATAL_ERROR "CMake step for foonathan-memory failed: ${result}")
endif ()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
		RESULT_VARIABLE result
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/foonathan-memory-download)
if (result)
	message(FATAL_ERROR "Build step for foonathan-memory failed: ${result}")
endif ()

# Add foonathan-memory directly to our build.
add_subdirectory(
		${CMAKE_CURRENT_BINARY_DIR}/foonathan-memory-src
		${CMAKE_CURRENT_BINARY_DIR}/foonathan-memory-build
		EXCLUDE_FROM_ALL
)

set(FOONATHAN_MEMORY_BUILD_EXAMPLES OFF)
set(FOONATHAN_MEMORY_BUILD_TESTS OFF)
set(FOONATHAN_MEMORY_BUILD_TOOLS ON)
