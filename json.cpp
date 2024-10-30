#include "json.hpp"
#include "lex_func.hpp"

#include <chrono>
#include <format>
#include <iostream>
#include <string>

#include "parse_func.hpp"

namespace json {
  int skip_whitespace(std::string_view raw_json, int index);
  std::string JSONTokenType_to_string(JSONTokenType jtt);

  std::tuple<JSONValue, std::string> parse(std::string_view source) {
    auto [tokens, error] = json::lex(source);
    if (!error.empty()) {
      return {{}, error};
    }

    auto [ast, _, error1] = json::parse(tokens);
    return {ast, error1};
  }

  // TODO: make std::string a more powerful error type. Not sure how to do this yet in C++
  std::tuple<std::vector<JSONToken>, std::string> lex(std::string_view raw_json) {
    std::vector<JSONToken> tokens;

    // All tokens will store a pointer to the original source string for debugging purposes
    // Tokens include an index which is used to identify its offset from the start of the string
    auto original_copy = std::make_shared<std::string>(raw_json);

    // function pointers that will attempt to parse
    // TODO: Potentially fix this to use std::visit with variants
    auto generic_lexers = {lex_syntax, lex_string, lex_number, lex_null, lex_true, lex_false};
    for (int i{}; i < static_cast<int>(std::ssize(raw_json)); i++) {
      // Skip past whitespace, this feels unnecessarily complicated
      // TODO: Potentially Fix this
      if (auto new_index = skip_whitespace(raw_json, i); i != new_index) {
        i = new_index - 1;
        continue;
      }

      bool found{};
      // std::cout << "current index: " << raw_json[i] << std::endl;
      // TODO: Fix this heavy nesting. It doesn't look too good
      for (auto lexer: generic_lexers) {
        if (auto [token, new_index, error] = lexer(raw_json, i); i != new_index) {
          if (!error.empty())
            return {{}, error};

          tokens.push_back(token);
          i = new_index - 1;
          found = true;
          break;
        }
      }

      // std::cout << "new " << raw_json[i] << std::endl;

      if (found)
        continue;

      return {{}, format_error_json("Unable to lex", raw_json, i)};
    }

    return {tokens, ""};
  }

  std::tuple<JSONValue, int, std::string> parse(const std::vector<JSONToken> &tokens, int index) {
    auto token = tokens[index];
    switch (token.type) {
      case JSONTokenType::Number: {
        auto n = std::stod(token.value);
        return {JSONValue(n), index + 1, ""};
      }
      case JSONTokenType::Boolean:
        return {JSONValue(token.value == "true"), index + 1, ""};
      case JSONTokenType::Null:
        return {JSONValue(), index + 1, ""};
      case JSONTokenType::String:
        return {JSONValue(token.value), index + 1, ""};
      case JSONTokenType::Syntax:
        if (token.value == "[") {
          auto [array, new_index, error] = parse_array(tokens, index + 1);
          return {JSONValue(array), new_index, error};
        }

        if (token.value == "{") {
          auto [object, new_index, error] = parse_object(tokens, index + 1);
          return {JSONValue(object), new_index, error};
        }
    }

    return {JSONValue(), index, format_parse_error("Failed to parse", token)};
  }

  std::string deparse(const JSONValue &v, std::string whitespace) {
    return std::visit(
        [&whitespace]<typename T0>(const T0 &value) -> std::string {
          using T = std::decay_t<T0>;

          if constexpr (std::is_same_v<T, std::monostate>) {
            return "null";
          } else if constexpr (std::is_same_v<T, std::string>) {
            return "\"" + value + "\"";
          } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(value);
          } else if constexpr (std::is_same_v<T, bool>) {
            return value ? "true" : "false";
          } else if constexpr (std::is_same_v<T, std::vector<JSONValue>>) {
            std::string s = "[\n";
            for (size_t i = 0; i < value.size(); i++) {
              s += whitespace + "  " + json::deparse(value[i], whitespace + "  ");
              if (i < value.size() - 1) {
                s += ",";
              }
              s += "\n";
            }
            return s + whitespace + "]";
          } else if constexpr (std::is_same_v<T, std::map<std::string, JSONValue>>) {
            std::string s = "{\n";
            size_t i = 0;
            for (const auto &[key, val]: value) {
              s += whitespace + "  " + "\"" + key + "\": " + json::deparse(val, whitespace + "  ");
              if (i < value.size() - 1) {
                s += ",";
              }
              s += "\n";
              i++;
            }
            return s + whitespace + "}";
          }
          return "";
        },
        v.value);
  }

  std::string JSONTokenType_to_string(JSONTokenType jtt) {
    switch (jtt) {
      case JSONTokenType::String:
        return "String";
      case JSONTokenType::Number:
        return "Number";
      case JSONTokenType::Syntax:
        return "Syntax";
      case JSONTokenType::Boolean:
        return "Boolean";
      case JSONTokenType::Null:
        return "Null";
    }

    // this path shouldn't be reached
    return "ERROR: NEGLECTED";
  }

  std::string format_parse_error(std::string_view base, JSONToken token) {
    std::ostringstream s;
    s << "Unexpected token '" << token.value << "', type '" << JSONTokenType_to_string(token.type) << "', index ";
    s << std::endl << base;
    return format_error_json(s.str(), token.full_source, token.location);
  }

  std::string format_error_json(std::string_view base, std::string_view source, int error_index) {
    int counter{}, line{1}, column{};
    std::string last_line;
    std::string whitespace;

    // Process characters up to the error index
    // TODO: Why?
    while (counter != error_index) {
      auto c{source[counter]};

      if (c == '\n') {
        line++;
        column = 0;
        last_line.clear();
        whitespace.clear();
      } else if (c == '\t') {
        column++;
        last_line += "\t";
        whitespace += "\t";
      } else {
        column++;
        last_line += c;
        whitespace += " ";
      }

      counter++;
    }

    // Do the last line
    while (counter < static_cast<int>(std::ssize(source))) {
      auto c{source[counter]};
      if (c == '\n')
        break;

      last_line += c;
      counter++;
    }

    return std::format("{} at line {}, column {}\n{}\n{}^", base, line, column, last_line, whitespace);
  }

  // skips over whitespace
  int skip_whitespace(std::string_view raw_json, int index) {
    while (index < static_cast<int>(std::ssize(raw_json)) && std::isspace(raw_json[index]))
      index++;

    // std::cout << raw_json[index] << std::endl;
    return index;
  }
} // namespace json
