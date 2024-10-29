#include "json.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
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

    // Process the JSON
    auto [tokens, error] = json::lex(in);
    if (error.size()) {
        std::cerr << error << std::endl;
        return 1;
    }

    for (auto t : tokens) {
        std::cout << "Token: " << t.value << std::endl;
    }

    return 0;
}