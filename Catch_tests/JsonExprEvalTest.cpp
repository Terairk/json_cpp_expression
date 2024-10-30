// test_json_eval.cpp
#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include "evaluator.hpp"
#include "expr_parser.hpp"
#include "json.hpp"

using json::JSONValue;

// Helper function to simulate command-line evaluation
JSONValue evaluate_expression(const std::string& json_str, const std::string& expression) {
    auto [json_ast, json_error] = json::parse(json_str);
    REQUIRE(json_error.empty());

    ExprParser parser;
    auto expr = parser.parse(expression);
    Evaluator evaluator(json_ast);
    return evaluator.evaluate(expr);
}

TEST_CASE("Basic JSON path expressions", "[json_eval]") {
    const std::string test_json = R"({"a": { "b": [ 1, 2, { "c": "test" }, [11, 12] ]}})";

    SECTION("Simple array access") {
        auto result = evaluate_expression(test_json, "a.b[1]");
        REQUIRE(std::get<double>(result.value) == 2.0);
    }

    SECTION("Nested object access") {
        auto result = evaluate_expression(test_json, "a.b[2].c");
        REQUIRE(std::get<std::string>(result.value) == "test");
    }

    SECTION("Full array access") {
        auto result = evaluate_expression(test_json, "a.b");
        auto& arr = std::get<std::vector<JSONValue>>(result.value);
        REQUIRE(arr.size() == 4);
        REQUIRE(std::get<double>(arr[0].value) == 1.0);
        REQUIRE(std::get<double>(arr[1].value) == 2.0);
    }
}

TEST_CASE("Dynamic subscript expressions", "[json_eval]") {
    const std::string test_json = R"({"a": { "b": [ 1, 2, { "c": "test" }, [11, 12] ]}})";

    SECTION("Expression in subscript") {
        auto result = evaluate_expression(test_json, "a.b[a.b[1]].c");
        REQUIRE(std::get<std::string>(result.value) == "test");
    }
}

TEST_CASE("Intrinsic functions", "[json_eval]") {
    const std::string test_json = R"({"a": { "b": [ 1, 2, { "c": "test" }, [11, 12] ]}})";

    SECTION("max function with array elements") {
        auto result = evaluate_expression(test_json, "max(a.b[0], a.b[1])");
        REQUIRE(std::get<double>(result.value) == 2.0);
    }

    SECTION("min function with nested array") {
        auto result = evaluate_expression(test_json, "min(a.b[3])");
        REQUIRE(std::get<double>(result.value) == 11.0);
    }

    SECTION("size function with object") {
        auto result = evaluate_expression(test_json, "size(a)");
        REQUIRE(std::get<double>(result.value) == 1.0);
    }

    SECTION("size function with array") {
        auto result = evaluate_expression(test_json, "size(a.b)");
        REQUIRE(std::get<double>(result.value) == 4.0);
    }

    SECTION("size function with string") {
        auto result = evaluate_expression(test_json, "size(a.b[2].c)");
        REQUIRE(std::get<double>(result.value) == 4.0);
    }
}

TEST_CASE("Number literals in expressions", "[json_eval]") {
    const std::string test_json = R"({"a": { "b": [ 1, 2, { "c": "test" }, [11, 12] ]}})";

    SECTION("max function with literals") {
        auto result = evaluate_expression(test_json, "max(a.b[0], 10, a.b[1], 15)");
        REQUIRE(std::get<double>(result.value) == 15.0);
    }
}

TEST_CASE("Error handling", "[json_eval]") {
    const std::string test_json = R"({"a": { "b": [ 1, 2, { "c": "test" }, [11, 12] ]}})";

    SECTION("Invalid JSON") {
        auto [_, json_error] = json::parse("{invalid json}");
        REQUIRE_FALSE(json_error.empty());
    }

    SECTION("Invalid path") {
        REQUIRE_THROWS(evaluate_expression(test_json, "a.nonexistent.field"));
    }

    SECTION("Invalid array index") {
        REQUIRE_THROWS(evaluate_expression(test_json, "a.b[999]"));
    }

    SECTION("Invalid function call") {
        REQUIRE_THROWS(evaluate_expression(test_json, "unknown_function(a.b)"));
    }

    SECTION("Type mismatch") {
        REQUIRE_THROWS(evaluate_expression(test_json, "max(a.b[2].c)")); // Trying to get max of a string
    }
}