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
    TEST_CASE("request")
    {
        std::string_view src = "POST /some/file%20name.html?with=parameter&and=another_one HTTP/1.1\r\n"
                               "host: some.host.name.com\r\n"
                               "accept: application/json\r\n"
                               "Custom-Header: custom value\r\n"
                               "\r\n"
                               "      ";

        Parser parser{MessageType::request};
        size_t offset = 0;
        size_t next_size = 0;
        while (!parser.finished()) {
            std::string_view next_input = src.substr(offset, next_size);
            size_t size = parser.input({reinterpret_cast<const std::byte*>(next_input.data()), next_input.size()});
            next_size += 10 - size;
            offset += size;
        }

        CHECK(offset == src.size() - 6);
        CHECK_THROWS(parser.response());
        Request& req = parser.request();

        CHECK(req.method == methods::post);
        CHECK(req.path == "/some/file%20name.html?with=parameter&and=another_one");
        CHECK(req.headers.size() == 3);
        CHECK(req.headers[0].name == header_fields::host);
        CHECK(req.headers[0].value == "some.host.name.com");
        CHECK(req.headers[1].name == header_fields::accept);
        CHECK(req.headers[1].value == "application/json");
        CHECK(req.headers[2].name == "Custom-Header");
        CHECK(req.headers[2].value == "custom value");
        CHECK(req.body.empty());
    }

    TEST_CASE("response")
    {
        std::string_view src = "HTTP/1.1 200 working as expected\r\n"
                               "server: some server name/0.0\r\n"
                               "content-type: application/json\r\n"
                               "content-length: 12\r\n"
                               "Custom-Header: custom value\r\n"
                               "\r\n"
                               "hello world!"
                               "      ";

        Parser parser{MessageType::response};
        size_t offset = 0;
        size_t next_size = 0;
        while (!parser.finished()) {
            std::string_view next_input = src.substr(offset, next_size);
            size_t size = parser.input({reinterpret_cast<const std::byte*>(next_input.data()), next_input.size()});
            next_size += 10 - size;
            offset += size;
        }

        CHECK(offset == src.size() - 6);
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
        CHECK(res.body.as_chars() == "hello world!");
    }

    TEST_CASE("chunked")
    {
        std::string_view src = "HTTP/1.1 299 chuchu\r\n"
                               "Transfer-Encoding: chunked\r\n"
                               "\r\n"
                               "5\r\n"
                               "hello\r\n"
                               "8\r\n"
                               " world! \r\n"
                               "1a\r\n"
                               "aaaaaaaaaaaaaaaaaaaaaaaaaa\r\n"
                               "0\r\n\r\n"
                               "      ";

        Parser parser{MessageType::response};
        size_t offset = 0;
        size_t next_size = 0;
        while (!parser.finished()) {
            std::string_view next_input = src.substr(offset, next_size);
            size_t size = parser.input({reinterpret_cast<const std::byte*>(next_input.data()), next_input.size()});
            next_size += 3 - size;
            offset += size;
        }

        CHECK(offset == src.size() - 6);
        CHECK_THROWS(parser.request());
        Response& res = parser.response();

        CHECK(res.status_code == 299);
        CHECK(res.status_message == "chuchu");
        CHECK(res.headers.size() == 1);
        CHECK(res.headers[0].name == "Transfer-Encoding");
        CHECK(res.headers[0].value == "chunked");
        CHECK(res.body.as_chars() == "hello world! aaaaaaaaaaaaaaaaaaaaaaaaaa");
    }
}

}  // namespace algue::http::test
