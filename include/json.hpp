#pragma once

#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

namespace json {
  enum class JSONTokenType { String, Number, Syntax, Boolean, Null };

  struct JSONToken {
    std::string value;
    JSONTokenType type;
    int location;
    std::string_view full_source;
  };

  struct JSONValue {
    using variant_type = std::variant<std::monostate, // represents null
                                      std::string, double, bool,
                                      std::vector<JSONValue>, // array value
                                      std::map<std::string, JSONValue>>;

    variant_type value;

    JSONValue() : value(std::monostate{}) {}

    // Explicit constructors for each type, can't just do std::forward unfortunately
    // get some compile time errors unfortunately
    explicit JSONValue(const std::string &v) : value(v) {}
    explicit JSONValue(std::string &&v) : value(std::move(v)) {}
    explicit JSONValue(const char *v) : value(std::string(v)) {}
    explicit JSONValue(double v) : value(v) {}
    explicit JSONValue(bool v) : value(v) {}
    explicit JSONValue(const std::vector<JSONValue> &v) : value(v) {}
    explicit JSONValue(std::vector<JSONValue> &&v) : value(std::move(v)) {}
    explicit JSONValue(const std::map<std::string, JSONValue> &v) : value(v) {}
    explicit JSONValue(std::map<std::string, JSONValue> &&v) : value(std::move(v)) {}
  };

  std::tuple<std::vector<JSONToken>, std::string> lex(std::string_view);
  std::tuple<JSONValue, int, std::string> parse(const std::vector<JSONToken> &, int index = 0);

  // Does both the lexing and parsing. Highest level function
  std::tuple<JSONValue, std::string> parse(std::string_view);

  // this deparse method could potentially be done through std::visit
  // worth considering
  std::string deparse(const JSONValue &, std::string whitespace = "");

  std::string format_error_json(std::string_view base, std::string_view source, int error_index);
  std::string format_parse_error(std::string_view base, JSONToken token);
} // namespace json
