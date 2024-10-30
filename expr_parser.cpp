#include "expr_parser.hpp"

#include <cctype>
#include <stdexcept>

bool ExprParser::isAtEnd() const { return current >= expr.length(); }

char ExprParser::peek() const {
  if (isAtEnd())
    return '\0';
  return expr[current];
}

char ExprParser::advance() { return expr[current++]; }

void ExprParser::skipWhitespace() {
  while (isspace(peek()))
    advance();
}

bool ExprParser::match(char expected) {
  if (isAtEnd() || peek() != expected)
    return false;
  advance();
  return true;
}

std::string ExprParser::parseFunctionName() {
  std::string name;
  while (isalpha(peek())) {
    name += advance();
  }
  return name;
}

std::unique_ptr<Expr> ExprParser::parseNumber() {
  std::string num;
  while (isdigit(peek())) {
    num += advance();
  }
  return std::make_unique<LiteralExpr>(json::JSONValue(std::stod(num)));
}

std::vector<std::unique_ptr<Expr>> ExprParser::parseArguments() {
  std::vector<std::unique_ptr<Expr>> args;

  if (!match('('))
    throw std::runtime_error("Expected '(' after function name");

  if (!match(')')) { // If not empty arguments
    do {
      skipWhitespace();
      args.push_back(parseExpr());
      skipWhitespace();
    } while (match(','));

    if (!match(')'))
      throw std::runtime_error("Expected ')' after arguments");
  }

  return args;
}

std::unique_ptr<Expr> ExprParser::parsePath() {
  std::vector<std::variant<std::string, std::unique_ptr<Expr>>> segments;

  // Parse first segment
  std::string segment;
  while (isalnum(peek())) {
    segment += advance();
  }
  if (segment.empty()) {
    throw std::runtime_error("Expected path segment");
  }
  segments.emplace_back(segment);

  while (!isAtEnd()) {
    if (match('.')) {
      // Parse dot notation
      segment.clear();
      while (isalnum(peek())) {
        segment += advance();
      }
      if (segment.empty()) {
        throw std::runtime_error("Expected identifier after '.'");
      }
      segments.emplace_back(segment);
    } else if (match('[')) {
      // Parse array index
      skipWhitespace();
      auto indexExpr = parseExpr(); // Use parseExpr directly
      skipWhitespace();
      if (!match(']')) {
        throw std::runtime_error("Expected ']'");
      }
      segments.emplace_back(std::move(indexExpr));
    } else {
      break;
    }
  }

  int x = 0;
  return std::make_unique<PathExpr>(std::move(segments));
}

std::unique_ptr<Expr> ExprParser::parseExpr() {
  skipWhitespace();

  // Parse function call
  if (isalpha(peek())) {
    std::string name = parseFunctionName();
    if (peek() == '(') {
      // The function below handles the '(' and ')'
      auto args = parseArguments();
      return std::make_unique<FunctionExpr>(name, std::move(args));
    }
    // If not a function, treat as path
    current -= name.length();
    return parsePath();
  }

  // Parse number literal
  if (isdigit(peek())) {
    return parseNumber();
  }

  throw std::runtime_error("Unexpected character");
}

std::unique_ptr<Expr> ExprParser::parse(const std::string &expression) {
  expr = expression;
  current = 0;
  return parseExpr();
}
