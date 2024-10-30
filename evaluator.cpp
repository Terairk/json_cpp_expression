#include "evaluator.hpp"

#include <functional>
#include <limits>
#include <stdexcept>
#include "expr.hpp"

Evaluator::Evaluator(const json::JSONValue &root) : root(root) {}

json::JSONValue Evaluator::evaluate(const std::unique_ptr<Expr> &expr) const { return expr->accept(*this); }

json::JSONValue Evaluator::visitLiteral(const LiteralExpr &expr) const { return expr.value; }

json::JSONValue Evaluator::visitPath(const PathExpr &expr) const { return resolvePath(expr.segments); }

json::JSONValue Evaluator::visitFunction(const FunctionExpr &expr) const {
  std::vector<json::JSONValue> args;
  args.reserve(expr.arguments.size());
  for (const auto &arg: expr.arguments) {
    args.push_back(arg->accept(*this));
  }

  if (expr.name == "min") {
    return evaluateMin(args);
  }

  if (expr.name == "max") {
    return evaluateMax(args);
  }

  if (expr.name == "size") {
    return evaluateSize(args);
  }
  throw std::runtime_error("Unknown function: " + expr.name);
}

// This is where the brain of the evaluating is done
// Resolves a JSON path expression (e.g., "a.b[1]") to its corresponding value
// segments: Vector of path components, alternating between string keys and array indices
//          e.g., for "a.b[1]", segments contains ["a", "b", Expression(1)] or roughly similar
json::JSONValue
Evaluator::resolvePath(const std::vector<std::variant<std::string, std::unique_ptr<Expr>>> &segments) const {
  // Initialize with a copy of the root JSON value
  // This ensures we don't modify the original data
  json::JSONValue current = root;

  // Process each segment of the path sequentially
  for (const auto &segment: segments) {
    if (std::holds_alternative<std::string>(segment)) {
      // Handle object key access (e.g., the "a" in "a.b")
      const auto &key = std::get<std::string>(segment);

      // Try to get the current value as an object
      if (auto *obj = std::get_if<std::map<std::string, json::JSONValue>>(&current.value)) {
        // Look up the key in the object
        auto it = obj->find(key);
        if (it == obj->end()) {
          throw std::runtime_error("Key not found: " + key);
        }
        // Create a new JSONValue from the found value
        // This maintains proper variant wrapping
        current = json::JSONValue(it->second);
      } else {
        // If current value is not an object, we can't access it with a key
        throw std::runtime_error("Invalid path: expected object");
      }
    } else {
      // Handle array index access (e.g., the "[1]" in "a.b[1]")
      const auto &indexExpr = std::get<std::unique_ptr<Expr>>(segment);
      // Evaluate the index expression (could be a literal or a complex expression)
      auto indexValue = indexExpr->accept(*this);

      // Try to get the current value as an array
      if (auto *arr = std::get_if<std::vector<json::JSONValue>>(&current.value)) {
        if (auto *index = std::get_if<double>(&indexValue.value)) {
          auto idx = static_cast<size_t>(*index);
          // Check for array bounds
          if (idx >= arr->size()) {
            throw std::runtime_error("Array index out of bounds");
          }
          // Create a new JSONValue from the array element
          // This maintains proper variant wrapping
          current = json::JSONValue((*arr)[idx]);
        } else {
          // Index expression didn't evaluate to a number
          throw std::runtime_error("Invalid array index type");
        }
      } else {
        // If current value is not an array, we can't access it with an index
        throw std::runtime_error("Invalid path: expected array");
      }
    }
  }

  // Return the final resolved value
  return current;
}

namespace {
  // Helper function that handles both min and max operations
  json::JSONValue helperMinMax(const std::vector<json::JSONValue> &args, const std::string &opName, double initialValue,
                               const std::function<double(double, double)> &compareOp) {
    if (args.empty())
      throw std::runtime_error(opName + " requires at least one argument");

    double result = initialValue;
    // can only do max on a double or an Array
    for (const auto &arg: args) {
      if (auto *num = std::get_if<double>(&arg.value)) {
        result = compareOp(result, *num);
      } else if (auto *arr = std::get_if<std::vector<json::JSONValue>>(&arg.value)) {
        if (arr->empty())
          continue;

        for (const auto &element: *arr) {
          if (auto *arrNum = std::get_if<double>(&element.value)) {
            result = compareOp(result, *arrNum);
          } else {
            throw std::runtime_error("Array elements must be numbers for " + opName + " operation");
          }
        }
      } else {
        throw std::runtime_error("Can't use " + opName + " on a non-double value");
      }
    }
    return json::JSONValue(result);
  }
} // anonymous namespace

json::JSONValue Evaluator::evaluateMin(const std::vector<json::JSONValue> &args) {
  return helperMinMax(args, "min", std::numeric_limits<double>::max(),
                      [](double a, double b) { return std::min(a, b); });
}

json::JSONValue Evaluator::evaluateMax(const std::vector<json::JSONValue> &args) {
  return helperMinMax(args, "max", std::numeric_limits<double>::lowest(),
                      [](double a, double b) { return std::max(a, b); });
}

json::JSONValue Evaluator::evaluateSize(const std::vector<json::JSONValue> &args) {
  if (args.size() != 1)
    throw std::runtime_error("size requires exactly one argument");


  // The following returns double because it's more convenient to line up with JSONValue
  const auto &arg = args[0];
  if (auto *arr = std::get_if<std::vector<json::JSONValue>>(&arg.value)) {
    return json::JSONValue(static_cast<double>(arr->size()));
  }

  if (auto *obj = std::get_if<std::map<std::string, json::JSONValue>>(&arg.value)) {
    return json::JSONValue(static_cast<double>(obj->size()));
  }

  if (auto *str = std::get_if<std::string>(&arg.value)) {
    return json::JSONValue(static_cast<double>(str->size()));
  }

  throw std::runtime_error("size argument must be array, object, or string");
}
