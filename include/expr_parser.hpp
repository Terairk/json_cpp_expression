#pragma once
#include <memory>
#include <string>
#include <vector>
#include "expr.hpp"

class ExprParser {
private:
  std::string expr;
  size_t current = 0;

  [[nodiscard]] bool isAtEnd() const;
  [[nodiscard]] char peek() const;
  char advance();
  void skipWhitespace();
  bool match(char expected);
  std::string parseFunctionName();
  std::unique_ptr<Expr> parseNumber();
  std::vector<std::unique_ptr<Expr>> parseArguments();
  std::unique_ptr<Expr> parsePath();
  std::unique_ptr<Expr> parseExpr();

public:
  ExprParser() = default;
  std::unique_ptr<Expr> parse(const std::string &expression);
};
