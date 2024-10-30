cmake_minimum_required(VERSION 3.15)
project(json_eval)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(json_eval main.cpp
		json.cpp
		lex_func.cpp
		parse_func.cpp
		expr.cpp
		evaluator.cpp
		expr_parser.cpp
		include/json.hpp
		include/lex_func.hpp
		include/parse_func.hpp
		include/expr.hpp
		include/expr_visitor.hpp
		include/evaluator.hpp
		include/expr_parser.hpp)

target_include_directories(json_eval PRIVATE include)
target_compile_options(json_eval PRIVATE -Wall -Wextra)
add_subdirectory(Catch_tests)