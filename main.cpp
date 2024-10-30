

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include "evaluator.hpp"
#include "expr_parser.hpp"
#include "json.hpp"
int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <json_file> <expression>" << std::endl;
    return 1;
  }

  // Open and read the JSON file
  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << argv[1] << std::endl;
    return 1;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();

  // Parse the JSON file
  auto [json_ast, json_error] = json::parse(buffer.str());
  if (!json_error.empty()) {
    std::cerr << "JSON parse error: " << json_error << std::endl;
    return 1;
  }

  // Parse the expression
  ExprParser parser;
  try {
    auto expr = parser.parse(argv[2]);

    // Evaluator uses the json_ast as the basis for querying,
    // expr is the thing to evaluate (uses the json_ast as the tree to search)
    Evaluator evaluator(json_ast);
    auto result = evaluator.evaluate(expr);
    std::cout << json::deparse(result) << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Expression evaluation error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
