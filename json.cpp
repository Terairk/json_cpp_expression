#include "json.hpp"
#include "lex_func.hpp"

#include <chrono>
#include <format>
#include <string>

namespace json {
  int skip_whitespace(std::string_view raw_json, int index);

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
      // TODO: Fix this heavy nesting. It doesn't look too good
      for (auto lexer: generic_lexers) {
        if (auto [token, new_index, error] = lexer(raw_json, i); i != new_index) {
          if (error.length())
            return {{}, error};

          token.full_source = original_copy;
          tokens.push_back(token);
          i = new_index - 1;
          found = true;
          break;
        }
      }

      if (found)
        continue;

      return {{}, format_error_json("Unable to lex", raw_json, i)};
    }

    return {tokens, ""};
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
    while (counter < source.length()) {
      auto c{source[counter]};
      if (c == '\n')
        break;

      last_line += c;
      counter++;
    }

    return std::format("{} at line {}, column {}\n{}\n{}^",
      base,
      line,
      column,
      last_line,
      whitespace);
  }

  // skips over whitespace
  int skip_whitespace(std::string_view raw_json, int index) {
    while (index < static_cast<int>(std::ssize(raw_json)) && std::isspace(raw_json[index]))
      index++;

    return index;
  }
} // namespace json
