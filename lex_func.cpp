//
// Created by terp on 29/10/24.
//

#include "lex_func.hpp"

namespace json {
   std::tuple<JSONToken, int, std::string> lex_string(std::string_view raw_json, int original_index) {
    int index{original_index};
    JSONToken token{"", JSONTokenType::String, index};
    std::string value{};
    auto c = raw_json[index];
    if (c != '"')
      return {token, original_index, ""};

    index++;
    // now inside the quotes

    //TODO: handle nested quotes
    while (c = raw_json[index], c != '"') {
      if (index == static_cast<int>(std::ssize(raw_json))) {
        return {token, index, format_error_json("Unexpected EOF while lexing string", raw_json, index)};
      }

      token.value += c;
      index++;
    }
    // go pass the closing "
    index++;
    return {token, index, ""};
  }


  // returns a string representation of the Number, we ensure its valid here
  std::tuple<JSONToken, int, std::string> lex_number(std::string_view raw_json, int index) {
     JSONToken token{"", JSONTokenType::Number, index};

     int raw_json_size = static_cast<int>(std::ssize(raw_json));

     // Check for minus sign
     if (index < raw_json_size && raw_json[index] == '-')
       index++;


     // Ensure there's at least one digit
     if (index >= raw_json_size || !std::isdigit(raw_json[index])) {
       // Invalid Number so stop here
       return {token, index, ""};
     }

     // Parse integer part
     auto start = static_cast<size_t>(index);
     while (index < raw_json_size && std::isdigit(raw_json[index]))
       index++;

     // Check for decimal point
     if (index < raw_json_size && raw_json[index] == '.') {
       index++;

     }
   }
}