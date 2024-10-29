#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <map>
#include <variant>

namespace json {
  enum class JSONTokenType { String, Number, Object, Array, Boolean, Null };

  struct JSONToken {
    std::string value;
    JSONTokenType type;
    int location;
    std::shared_ptr<std::string> full_source;
  };

  struct JSONValue {
    using variant_type = std::variant<
      std::monostate,         // represents null
      std::string,
      double,
      bool,
      std::vector<JSONValue>, // array value
      std::map<std::string, JSONValue>
    >;

    variant_type value;

    JSONValue() : value(std::monostate{}) {}

    template<typename T>
    explicit JSONValue(T&& v) : value(std::forward<T>(v)) {}

  };


  std::tuple<std::vector<JSONToken>, std::string> lex(std::string);
  std::tuple<JSONValue, int, std::string> parse(std::vector<JSONToken>, int index = 0);

  // Does both the lexing and parsing. Highest level function
  std::tuple<JSONValue, std::string> parse(std::string);

  // this deparse method could potentially be done through std::visit
  // worth considering
  std::string deparse(JSONValue, std::string whitespace = "");
  std::string format_error_json(std::string_view base, std::string_view source, int error_index);
}