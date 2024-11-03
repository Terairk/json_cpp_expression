#pragma once
#include "json.hpp"
namespace json {
  std::tuple<std::vector<JSONValue>, int, std::string> parse_array(const std::vector<JSONToken> &tokens, int index);

  std::tuple<std::map<std::string, JSONValue>, int, std::string> parse_object(const std::vector<JSONToken> &tokens, int
  index);
} // namespace json
