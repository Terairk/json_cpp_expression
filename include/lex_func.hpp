#pragma once
#include "json.hpp"

namespace json {
  std::tuple<JSONToken, int, std::string> lex_string(std::string_view raw_json, int original_index);
  std::tuple<JSONToken, int, std::string> lex_number(std::string_view raw_json, int index);

  std::tuple<JSONToken, int, std::string> lex_syntax(std::string_view raw_json, int original_index);

  std::tuple<JSONToken, int, std::string> lex_null(std::string_view raw_json, int original_index);

  std::tuple<JSONToken, int, std::string> lex_true(std::string_view raw_json, int original_index);

  std::tuple<JSONToken, int, std::string> lex_false(std::string_view raw_json, int original_index);

  std::tuple<JSONToken, int, std::string> lex_intrinsic(std::string_view raw_json, int original_index);
} // namespace json
