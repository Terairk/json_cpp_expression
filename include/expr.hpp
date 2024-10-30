#pragma once
#include "json.hpp"
class ExprVisitor;

// Base Expr Class
struct Expr {
  virtual ~Expr() = default;
  [[nodiscard]] virtual json::JSONValue accept(const ExprVisitor &visitor) const = 0;
};

// Literal value (numbers, strings, etc.)
struct LiteralExpr : Expr {
  json::JSONValue value;
  explicit LiteralExpr(json::JSONValue v);
  [[nodiscard]] json::JSONValue accept(const ExprVisitor &visitor) const override;
};

// Path expression (a.b[1])
// "a", "b", "Expr(1)"
struct PathExpr : Expr {
  std::vector<std::variant<std::string, std::unique_ptr<Expr>>> segments;
  explicit PathExpr(std::vector<std::variant<std::string, std::unique_ptr<Expr>>> segs);
  [[nodiscard]] json::JSONValue accept(const ExprVisitor &visitor) const override;
};

// Function call expression - intrinsic functions (min, max, size)
struct FunctionExpr : Expr {
  std::string name;
  std::vector<std::unique_ptr<Expr>> arguments;
  explicit FunctionExpr(std::string n, std::vector<std::unique_ptr<Expr>> args);
  [[nodiscard]] json::JSONValue accept(const ExprVisitor &visitor) const override;
};
