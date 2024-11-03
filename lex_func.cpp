//
// Created by terp on 29/10/24.
//

#include "lex_func.hpp"

#include <iostream>
#include <sstream>

namespace json {
  static std::tuple<JSONToken, int, std::string> lex_keyword(std::string_view raw_json, std::string_view keyword,
                                                             JSONTokenType type, int original_index) {
    int index = original_index;
    JSONToken token{"", type, index, raw_json};
    int raw_json_length = static_cast<int>(std::ssize(raw_json));
    while (index != raw_json_length && keyword[index - original_index] == raw_json[index]) {
      index++;
    }

    if (index - original_index == static_cast<int>(std::ssize(keyword))) {
      token.value = keyword;
    }
    return std::make_tuple( token, index, "" );
  }

  std::tuple<JSONToken, int, std::string> lex_string(std::string_view raw_json, int original_index) {
    int index{original_index};
    JSONToken token{"", JSONTokenType::String, index, raw_json};
    auto c = raw_json[index];

    if (c != '"') {
      return std::make_tuple( token, original_index, "" );
    }

    index++; // move past opening quote

    while (index < static_cast<int>(std::ssize(raw_json))) {
      c = raw_json[index];

      if (c == '"') {
        // Found end of string
        index++; // move past closing quote
        return std::make_tuple( token, index, "" );
      }

      if (c == '\\') {
        // Handle escape sequences
        if (index + 1 >= static_cast<int>(std::ssize(raw_json))) {
          return {token, index, format_error_json("Unexpected EOF after backslash", raw_json, index)};
        }

        index++; // move to character after backslash
        c = raw_json[index];

        switch (c) {
          case '"':
            token.value += '"';
            break;
          case '\\':
            token.value += '\\';
            break;
          case '/':
            token.value += '/';
            break;
          case 'b':
            token.value += '\b';
            break;
          case 'f':
            token.value += '\f';
            break;
          case 'n':
            token.value += '\n';
            break;
          case 'r':
            token.value += '\r';
            break;
          case 't':
            token.value += '\t';
            break;
          case 'u':
            // Handle Unicode escape sequences
            if (index + 4 >= static_cast<int>(std::ssize(raw_json))) {
              return {token, index, format_error_json("Incomplete Unicode escape sequence", raw_json, index)};
            }
            // TODO: Implement Unicode escape sequence handling
            // For now, just skip the next 4 characters
            index += 4;
            break;
          default:
            return {token, index, format_error_json("Invalid escape sequence", raw_json, index)};
        }
      } else {
        token.value += c;
      }

      index++;
    }

    return {token, index, format_error_json("Unterminated string", raw_json, index)};
  }

  std::tuple<JSONToken, int, std::string> lex_number(std::string_view raw_json, int index) {
    JSONToken token{"", JSONTokenType::Number, index, raw_json};
    std::string_view slice = raw_json.substr(index);

    // Find the length of the number in the original string
    size_t num_length = 0;
    bool has_digit = false;

    // Handle optional minus sign
    if (num_length < slice.length() && slice[num_length] == '-') {
      num_length++;
    }

    // Process digits before decimal point
    while (num_length < slice.length() && std::isdigit(slice[num_length])) {
      has_digit = true;
      num_length++;
    }

    // Handle decimal point and following digits
    if (num_length < slice.length() && slice[num_length] == '.') {
      num_length++;
      while (num_length < slice.length() && std::isdigit(slice[num_length])) {
        has_digit = true;
        num_length++;
      }
    }

    // Handle scientific notation
    if (num_length < slice.length() && (slice[num_length] == 'e' || slice[num_length] == 'E')) {
      num_length++;
      if (num_length < slice.length() && (slice[num_length] == '+' || slice[num_length] == '-')) {
        num_length++;
      }
      while (num_length < slice.length() && std::isdigit(slice[num_length])) {
        has_digit = true;
        num_length++;
      }
    }

    if (!has_digit) {
      return std::make_tuple( token, index, "" );
    }

    token.value = std::string(slice.substr(0, num_length));
    return {token, index + static_cast<int>(num_length), ""};
  }

  // Syntax elements are ( ',' -> ':' -> '{' -> '}' -> '[' -> ']')
  std::tuple<JSONToken, int, std::string> lex_syntax(std::string_view raw_json, int index) {
    JSONToken token{"", JSONTokenType::Syntax, index, raw_json};
    std::string value{};
    auto c = raw_json[index];
    if (c == '[' || c == ']' || c == '{' || c == '}' || c == ':' || c == ',') {
      token.value += c;
      index++;
    }

    return std::make_tuple( token, index, "" );
  }

  std::tuple<JSONToken, int, std::string> lex_null(std::string_view raw_json, int index) {
    return lex_keyword(raw_json, "null", JSONTokenType::Null, index);
  }

  std::tuple<JSONToken, int, std::string> lex_true(std::string_view raw_json, int index) {
    return lex_keyword(raw_json, "true", JSONTokenType::Boolean, index);
  }

  std::tuple<JSONToken, int, std::string> lex_false(std::string_view raw_json, int index) {
    return lex_keyword(raw_json, "false", JSONTokenType::Boolean, index);
  }
} // namespace json
