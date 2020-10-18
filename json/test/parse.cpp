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
        CHECK_THROWS(parse("+"));
        CHECK_THROWS(parse("-"));
        CHECK_THROWS(parse("-+1"));
        CHECK_THROWS(parse("--1"));
        CHECK_THROWS(parse("01"));
        CHECK_THROWS(parse("0."));
        CHECK_THROWS(parse("1e"));
        CHECK_THROWS(parse("1e+-1"));
        CHECK_THROWS(parse("1e--1"));
        CHECK_THROWS(parse("1e-+1"));
        CHECK_THROWS(parse("1z"));
        CHECK_THROWS(parse("1.z"));
        CHECK_THROWS(parse("1ez"));

        auto check = [](std::string_view input, Type expected_type, auto expected_value) {
            Value v = parse(input);
            CHECK(v.type() == expected_type);
            if constexpr (std::is_same_v<decltype(expected_value), double>) {
                CHECK(v.get<double>() - expected_value < std::numeric_limits<double>::epsilon());
            }
            else {
                CHECK(v.get<decltype(expected_value)>() == expected_value);
            }
        };

        check("-12", Type::signed_number, -12);
        check("-0", Type::signed_number, 0);
        check("0", Type::unsigned_number, 0);
        check("12", Type::unsigned_number, 12);
        check("0.02", Type::floating_point, 0.02);
        check("16.0", Type::floating_point, 16.0);
        check("16.005", Type::floating_point, 16.005);
        check("-14.0", Type::floating_point, -14.0);
        check("1e9", Type::unsigned_number, static_cast<unsigned long long>(1e9));
        check("-123e+10", Type::signed_number, static_cast<long long>(-123e10));
        check("1.23E-10", Type::floating_point, 1.23e-10);
    }

    TEST_CASE("number_overflow")
    {
        CHECK(parse("127").get<char>() == 127);
        CHECK_THROWS(parse("128").get<char>());
        CHECK(parse("-128").get<char>() == -128);
        CHECK_THROWS(parse("-129").get<char>());

        CHECK(parse("-1").get<int>() == -1);
        CHECK_THROWS(parse("-1").get<unsigned>());
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
        CHECK_THROWS(parse("["));
        CHECK_THROWS(parse("]"));
        CHECK_THROWS(parse("[0,1,2,3"));
        CHECK(parse("[1,2,3,null,true,]").as<Array>() == Array{{1u, 2u, 3u, null, true}});
        CHECK(parse("[1,2,3,[null,false,],]").as<Array>() == Array{{1u, 2u, 3u, Array{{null, false}}}});
    }

    TEST_CASE("object")
    {
        CHECK_THROWS(parse("{"));
        CHECK_THROWS(parse("}"));
        CHECK_THROWS(parse(R"({"a":0,"b})"));
        CHECK_THROWS(parse(R"({"a":0,"b"})"));
        CHECK_THROWS(parse(R"({"a":0,"b":})"));
        CHECK_THROWS(parse(R"({"a":0,"b":1)"));

        Object o;
        o["a"] = 0u;
        o["b"] = "hello";
        CHECK(parse(R"({"a":0,"b":"hello"})").as<Object>() == o);
        o["c"] = o;
        CHECK(parse(R"({"a":0,"b":"hello","c":{"a":0,"b":"hello"}})").as<Object>() == o);
        CHECK(parse(R"({"a":0,"b":"hello","c":{"a":0,"b":"hello"},},)").as<Object>() == o);
        CHECK(parse(R"(  {  "a"  :  0  ,  "b"  :  "hello"  ,  "c"  :  {  "a"  :  0,  "b"  :  "hello"  ,  }  ,  }  ,  )").as<Object>() == o);
    }
}

}  // namespace algue::json::test
