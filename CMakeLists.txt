cmake_minimum_required(VERSION 3.15)
project(json_cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(json_cpp main.cpp
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

target_include_directories(json_cpp PRIVATE include)
target_compile_options(json_cpp PRIVATE -Wall -Wextra)