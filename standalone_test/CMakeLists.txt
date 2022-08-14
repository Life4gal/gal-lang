project(
		gal-script-lang-standalone-test
		LANGUAGES CXX
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(
		${PROJECT_NAME}_SOURCE
		
		src/main.cpp
)

add_executable(
		${PROJECT_NAME}
		
		${${PROJECT_NAME}_SOURCE}
)

set_compile_options_private(${PROJECT_NAME})

target_link_libraries(
		${PROJECT_NAME}
		PRIVATE
		gal::GSL
)
