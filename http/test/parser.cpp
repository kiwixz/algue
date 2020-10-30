#include "http/parser.h"

#include <algorithm>
#include <iostream>
#include <string_view>
#include <vector>

#include <doctest/doctest.h>

#include "http/header_fields.h"
#include "http/methods.h"
#include "http/status.h"

namespace algue::http::test {

TEST_SUITE("parser")
{
    TEST_CASE("response")
    {
        std::string_view src = "HTTP/1.1 200 working as expected\r\n"
                               "server: some server name/0.0\r\n"
                               "content-type: application/json\r\n"
                               "content-length: 12\r\n"
                               "Custom-Header: custom value\r\n"
                               "\r\n"
                               "hello world!";

        Parser parser{MessageType::response};
        size_t offset = 0;
        size_t next_size = 0;
        while (!parser.finished()) {
            std::string_view next_input = src.substr(offset, next_size);
            size_t size = parser.input({reinterpret_cast<const std::byte*>(next_input.data()), next_input.size()});
            next_size += 10 - size;
            offset += size;
        }

        CHECK(offset == src.size());
        CHECK_THROWS(parser.request());
        Response& res = parser.response();

        CHECK(res.status_code == 200);
        CHECK(res.status_message == "working as expected");
        CHECK(res.headers.size() == 4);
        CHECK(res.headers[0].name == header_fields::server);
        CHECK(res.headers[0].value == "some server name/0.0");
        CHECK(res.headers[1].name == header_fields::content_type);
        CHECK(res.headers[1].value == "application/json");
        CHECK(res.headers[2].name == header_fields::content_length);
        CHECK(res.headers[2].value == "12");
        CHECK(res.headers[3].name == "Custom-Header");
        CHECK(res.headers[3].value == "custom value");
        CHECK(std::string_view{reinterpret_cast<const char*>(res.body.data()), res.body.size()} == "hello world!");
    }
}

}  // namespace algue::http::test
