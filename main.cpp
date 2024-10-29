#include "json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cerr << "Expected JSON file path as argument" << std::endl;
        return 1;
    }

    // Open the file
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    // Read the entire file into a string
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string in = buffer.str();

    // Close the file
    file.close();

    // Parse the JSON
    auto [ast, error] = json::parse(in);
    if (error.size()) {
        std::cerr << error << std::endl;
        return 1;
    }

    // Output the parsed JSON
    std::cout << json::deparse(ast);
    return 0;
}