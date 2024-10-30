# JSON Expression Parser + Evaluator

## Overview
A C++ implementation of a JSON parser and evaluator, developed as part of the "LLDB Debugger for Windows" task for JetBrains Internship. This project showcases modern C++ features including move constructors, std::format, std::string_view, and std::variant.

## Features
- JSON file parsing and evaluation
- Expression parsing capabilities
- Modern C++20 implementation
- Comprehensive test suite using Catch2

## Prerequisites
- C++20 compatible compiler
- Catch2 testing framework
- CMake build system (3.15 or later)

## Installation

### Installing Catch2
Follow the [official Catch2 installation instructions](https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md#installing-catch2-from-git-repository)
Requires CMake 3.15 (at least 3 but change the top level CMakeLists.txt file if you need 
something earlier than 3.15)
### Building the Project

#### Using CLion
1. Open the project in CLion
2. The configuration should be automatically set up
3. Build and run the main application

To configure tests in CLion:
1. Edit Configuration
2. Add New Configuration
3. Select Catch
4. Set tags: [json_eval]

#### Manual Build
# Or use gcc/g++ if preferred
```bash
mkdir build
cd build
CC=clang CXX=clang++ cmake .. 
cmake --build . -j $(nproc)
```

The main files should then be in:

### Main application: build/json_eval
### Test suite: build/Catch_tests/Catch_tests_run

### Usage
```bash
./json_eval <path_to_json> "<query>"
```
## Tools & Technologies
- C++20 standard library
- Modern C++ features
- Catch2 testing framework

## Future Improvements
- Implement recursive descent parser for expression evaluation with operator precedence (*, /, +, -)
- Alternative: Implement shunting yard algorithm for expression parsing
- Enhance error handling and reporting