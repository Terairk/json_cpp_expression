cmake_minimum_required(VERSION 3.15)
project(json_eval)

set(CMAKE_CXX_STANDARD 20)
# Done to support other editors
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Library target with core functionality
add_library(json_cpp STATIC
		json.cpp
		lex_func.cpp
		parse_func.cpp
		expr.cpp
		evaluator.cpp
		expr_parser.cpp
)

# Set include directories for the library
target_include_directories(json_cpp
		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set compile options for the library
target_compile_options(json_cpp
		PRIVATE
		-Wall
		-Wextra
)

# Create the main executable
add_executable(json_eval main.cpp)

# Link the library to the main executable
target_link_libraries(json_eval
		PRIVATE
		json_cpp
)

# Add the tests subdirectory
add_subdirectory(Catch_tests)