#include "json/dump.h"

#include <doctest/doctest.h>

namespace algue::json::test {

TEST_SUITE("dump")
{
    TEST_CASE("simple")
    {
        CHECK(dump({}) == "null");
        CHECK(dump(null) == "null");
        CHECK(dump(false) == "false");
        CHECK(dump(true) == "true");
        CHECK(dump(-12) == "-12");
        CHECK(dump(0) == "0");
        CHECK(dump(12) == "12");
        CHECK(dump(16.0) == "16");
        CHECK(dump(-14.0) == "-14");
        CHECK(dump(16.005) == "16.005");
        CHECK(dump(1e20) == "1e+20");
        CHECK(dump(1e-20) == "1e-20");
        CHECK(dump("") == R"("")");
        CHECK(dump("hello") == R"("hello")");

        CHECK_THROWS(dump(std::numeric_limits<float>::quiet_NaN()));
        CHECK_THROWS(dump(std::numeric_limits<double>::infinity()));

        Array a;
        a.push_back(12);
        a.push_back("hello");
        a.push_back(a);
        CHECK(dump(a) == R"([12,"hello",[12,"hello"]])");

        Object o;
        o["a"] = 12;
        o["b"] = "hello";
        CHECK((dump(o) == R"({"a":12,"b":"hello"})" || dump(o) == R"({"b":"hello","a":12})"));
    }

    TEST_CASE("indent")
    {
        Object o;
        o["a"] = 12;
        o["b"] = "hello";
        CHECK((dump(o, 0) == R"({"a": 12, "b": "hello"})" || dump(o, 0) == R"({"b": "hello", "a": 12})"));

        Array a;
        a.push_back(12);
        a.push_back("hello");
        a.push_back(a);
        CHECK(dump(a, 2) == "[\n  12,\n  \"hello\",\n  [\n    12,\n    \"hello\"\n  ]\n]\n");
    }

    TEST_CASE("format")
    {
        CHECK(format(R"([12,"hello",[12,"hello"]])", 2)
              == "[\n  12,\n  \"hello\",\n  [\n    12,\n    \"hello\"\n  ]\n]\n");
    }
}

}  // namespace algue::json::test
