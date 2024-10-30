#include "expr.hpp"
#include "expr_visitor.hpp"

LiteralExpr::LiteralExpr(json::JSONValue v) : value(std::move(v)) {}

json::JSONValue LiteralExpr::accept(const ExprVisitor &visitor) const { return visitor.visitLiteral(*this); }

PathExpr::PathExpr(std::vector<std::variant<std::string, std::unique_ptr<Expr>>> segs) : segments(std::move(segs)) {}

json::JSONValue PathExpr::accept(const ExprVisitor &visitor) const { return visitor.visitPath(*this); }

FunctionExpr::FunctionExpr(std::string n, std::vector<std::unique_ptr<Expr>> args) :
    name(std::move(n)), arguments(std::move(args)) {}

json::JSONValue FunctionExpr::accept(const ExprVisitor &visitor) const { return visitor.visitFunction(*this); }
