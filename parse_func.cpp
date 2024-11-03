

#include "parse_func.hpp"
#include "json.hpp"

namespace json {
  // Parses a JSON array and returns a tuple containing:
  // - Vector of parsed JSON values
  // - Next token index to process
  // - Error message (empty string if successful)
  std::tuple<std::vector<JSONValue>, int, std::string> parse_array(const std::vector<JSONToken> &tokens, int index) {
    std::vector<JSONValue> children{};

    const int tokens_size = static_cast<int>(std::ssize(tokens));

    // Process tokens until we reach the end
    while (index < tokens_size) {
      // Get current token
      const auto& currentToken = tokens[index];

      // Handle syntax tokens (brackets and commas)
      if (currentToken.type == JSONTokenType::Syntax) {
        // Check for closing bracket - end of array
        if (currentToken.value == "]") {
          return {children, index + 1, ""};
        }

        // Handle comma separators
        if (currentToken.value == ",") {
          index++;
        }
        // If we find a non-comma syntax token after elements exist,
        // return an error
        else if (static_cast<int>(children.size()) > 0) {
          return std::make_tuple(std::vector<JSONValue>{}, index, format_parse_error("Expected comma after element in array",
          currentToken));
        }
      }

      // Parse the next value in the array
      auto [child, new_index, error] = parse(tokens, index);

      // If parsing produced an error, propagate it upward
      if (!error.empty()) {
        return std::make_tuple(std::vector<JSONValue>{}, index, error);
      }

      // Add successfully parsed child to array
      children.push_back(child);
      index = new_index;
    }

    // If we reach here, we hit EOF before finding closing bracket
    return std::make_tuple(std::vector<JSONValue>{}, index, format_parse_error("Unexpected EOF while parsing array",
    tokens[index]) );
  }

  // Parses a JSON object and returns a tuple containing:
  // - Map of string keys to JSON values
  // - Next token index to process
  // - Error message (empty string if successful)

  using JSONMap = std::map<std::string, JSONValue>;
  std::tuple<std::map<std::string, JSONValue>, int, std::string> parse_object(const std::vector<JSONToken> &tokens,
                                                                              int index) {
    // Initialize empty map to store object key-value pairs
    JSONMap values{};

    const int tokens_size = static_cast<int>(std::ssize(tokens));

    while (index < tokens_size) {
      auto currentToken = tokens[index];

      // Handle syntax tokens (braces and commas)
      if (currentToken.type == JSONTokenType::Syntax) {
        // Check for closing brace - end of object
        if (currentToken.value == "}") {
          return std::make_tuple(values, index + 1, "");
        }

        // Handle comma separators
        if (currentToken.value == ",") {
          index++;
          currentToken = tokens[index];
        }
        // If we find a non-comma syntax token after elements exist
        else if (static_cast<int>(values.size()) > 0) {
          return {{}, index, format_parse_error("Expected comma after element in object", currentToken)};
        }
        // Invalid syntax at start of object
        else {
          return std::make_tuple(JSONMap{}, index, format_parse_error("Expected key-value pair or closing brace in object", currentToken));
        }
      }

      // Parse the object key
      auto [key, new_index, error] = parse(tokens, index);
      if (!error.empty()) {
        return std::make_tuple(JSONMap{}, index, error);
      }

      // Verify the key is a string
      if (!std::holds_alternative<std::string>(key.value)) {
        return std::make_tuple(JSONMap{}, index, format_parse_error("Expected string key in object", currentToken));
      }
      index = new_index;
      currentToken = tokens[index];

      // Verify and consume the colon separator
      if (!(currentToken.type == JSONTokenType::Syntax && currentToken.value == ":")) {
        return std::make_tuple(JSONMap{}, index, format_parse_error("Expected colon after key in object",
        currentToken));
      }
      index++;

      // Parse the object value
      auto [value, new_index1, error1] = parse(tokens, index);
      if (!error1.empty()) {
        return std::make_tuple(JSONMap{}, index, error1);
      }

      // Add the key-value pair to the map
      // key should be a string due to previous assertion
      values[std::get<std::string>(key.value)] = value;
      index = new_index1;
    }

    // Return successfully parsed object
    return std::make_tuple(values, index + 1, "");
  }
} // namespace json
