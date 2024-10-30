#pragma once

#include "json.hpp"

// Forward declarations
struct LiteralExpr;
struct PathExpr;
struct FunctionExpr;

class ExprVisitor {
public:
  virtual ~ExprVisitor() = default;
  [[nodiscard]] virtual json::JSONValue visitLiteral(const LiteralExpr &expr) const = 0;
  [[nodiscard]] virtual json::JSONValue visitPath(const PathExpr &expr) const = 0;
  [[nodiscard]] virtual json::JSONValue visitFunction(const FunctionExpr &expr) const = 0;
};
