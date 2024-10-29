#pragma once
#include "json.hpp"
class ExprVisitor;

// Base Expr Class
struct Expr {
    virtual ~Expr() = default;
    virtual json::JSONValue accept(ExprVisitor& visitor) const = 0;
};

// Literal value (numbers, strings, etc.)
struct LiteralExpr : Expr {
    json::JSONValue value;
    explicit LiteralExpr(json::JSONValue v) : value(std::move(v)) {}
    json::JSONValue accept(ExprVisitor& visitor) const override;
};

// Path expression (a.b[1])
struct PathExpr : Expr {
    std::vector<std::variant<std::string, std::unique_ptr<Expr>>> segments;
    json::JSONValue accept(ExprVisitor& visitor) const override;
};

// Function call expression - intrinsic functions (min, max, size)
struct FunctionExpr : Expr {
    std::string name;
    std::vector<std::unique_ptr<Expr>> arguments;
    json::JSONValue accept(ExprVisitor& visitor) const override;
};
