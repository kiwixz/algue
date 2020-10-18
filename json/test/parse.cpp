#include "json/parse.h"

#include <iostream>

#include <doctest/doctest.h>
#include <fmt/format.h>

namespace algue::json::test {

TEST_SUITE("parse")
{
    TEST_CASE("simple")
    {
        CHECK_THROWS(parse(""));
        CHECK_THROWS(parse("hello"));

        CHECK(parse("null").type() == Type::null);
        CHECK(parse("   null").type() == Type::null);
        CHECK(parse("null\n").type() == Type::null);
        CHECK(parse("\t  null  ").type() == Type::null);

        CHECK(parse("false").get<bool>() == false);
        CHECK(parse("true").get<bool>() == true);
    }

    TEST_CASE("number")
    {
        CHECK_THROWS(parse("+1"));
        CHECK_THROWS(parse("01"));
        CHECK_THROWS(parse("0."));
        CHECK_THROWS(parse("1e"));
        CHECK_THROWS(parse("1z"));
        CHECK_THROWS(parse("1.z"));
        CHECK_THROWS(parse("1ez"));

        auto check = [](std::string_view input, Type expected_type, auto expected_value) {
            Value v = parse(input);
            CHECK(v.type() == expected_type);
            CHECK(v.get<decltype(expected_value)>() == expected_value);
        };

        check("-12", Type::signed_number, -12);
        check("-0", Type::signed_number, 0);
        check("0", Type::unsigned_number, 0);
        check("12", Type::unsigned_number, 12);
        check("0.02", Type::floating_point, 0.02);
        check("16.0", Type::floating_point, 16.0);
        check("16.005", Type::floating_point, 16.005);
        check("-14.0", Type::floating_point, -14.0);
        // check("1e20", Type::floating_point, 1e20);
        // check("-123e+20", Type::floating_point, -123e20);
        // check("1.23E-20", Type::floating_point, 1.23e-20);
    }

    TEST_CASE("string")
    {
        CHECK_THROWS(parse(R"(")"));
        CHECK_THROWS(parse(R"("unfinished)"));
        CHECK_THROWS(parse("\"\r\""));
        CHECK_THROWS(parse("\"\n\""));
        CHECK_THROWS(parse(R"("\")"));
        CHECK_THROWS(parse(R"("\a")"));
        CHECK_THROWS(parse(R"("\\"bad)"));
        CHECK_THROWS(parse(R"("\\\\"bad)"));
        CHECK_THROWS(parse(R"("bad\\"bad)"));
        CHECK_THROWS(parse(R"("bad\\\\"bad)"));

        auto check = [](std::string_view input, std::string_view expected) {
            Value v = parse(fmt::format(R"("{}")", input));
            CHECK(v.type() == Type::string);
            CHECK(v.as<std::string>() == expected);
        };

        check("hello", "hello");
        check("\x7f", "\x7f");
        check(R"(\"abc)", R"("abc)");
        check(R"(\\\"abc)", R"(\"abc)");
        check(R"(abc\"abc)", R"(abc"abc)");
        check(R"(abc\\\"abc)", R"(abc\"abc)");
        check(R"(\\\/\b\f\n\r\t)", "\\/\b\f\n\r\t");
    }

    TEST_CASE("array")
    {
    }

    TEST_CASE("object")
    {
    }
}

}  // namespace algue::json::test
