cmake_minimum_required(VERSION 3.29)
project(json_cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(json_cpp main.cpp
		json.hpp
		json.cpp
		lex_func.cpp
		lex_func.hpp)

target_compile_options(json_cpp PRIVATE -Wall -Wextra -Werror)
