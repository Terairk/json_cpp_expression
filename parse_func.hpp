#pragma once
#include "json.hpp"
namespace json {
  std::tuple<std::vector<JSONValue>, int, std::string>
parse_array(std::vector<JSONToken> tokens, int index);

  std::tuple<std::map<std::string, JSONValue>, int, std::string>
parse_object(std::vector<JSONToken> tokens, int index);
}