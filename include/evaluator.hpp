#pragma once

#include "expr.hpp"
#include "expr_visitor.hpp"
#include "json.hpp"

class Evaluator : public ExprVisitor {
private:
  const json::JSONValue &root;
  [[nodiscard]] json::JSONValue
  resolvePath(const std::vector<std::variant<std::string, std::unique_ptr<Expr>>> &segments) const;
  static json::JSONValue evaluateMin(const std::vector<json::JSONValue> &args);
  static json::JSONValue evaluateMax(const std::vector<json::JSONValue> &args);
  static json::JSONValue evaluateSize(const std::vector<json::JSONValue> &args);

public:
  explicit Evaluator(const json::JSONValue &root);
  [[nodiscard]] json::JSONValue evaluate(const std::unique_ptr<Expr> &expr) const;
  [[nodiscard]] json::JSONValue visitLiteral(const LiteralExpr &expr) const override;
  [[nodiscard]] json::JSONValue visitPath(const PathExpr &expr) const override;
  [[nodiscard]] json::JSONValue visitFunction(const FunctionExpr &expr) const override;
};
