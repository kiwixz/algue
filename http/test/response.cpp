#include "http/response.h"

#include <algorithm>
#include <iostream>
#include <string_view>
#include <vector>

#include <doctest/doctest.h>
#include <kae/exception.h>

#include "http/header_fields.h"
#include "http/methods.h"

namespace algue::http::test {

TEST_SUITE("response")
{
    TEST_CASE("simple")
    {
        std::string_view src = "HTTP/1.1 200 working as expected\r\n"
                               "server: some server name/0.0\r\n"
                               "content-type: application/json\r\n"
                               "content-length: 12\r\n"
                               "Custom-Header: custom value\r\n"
                               "\r\n"
                               "hello world!";

        printf("s %zu\n", src.size());

        Response res;
        size_t total_read = 0;
        res.deserialize([&](kae::Span<std::byte> buffer) {
            if (total_read == src.size())
                throw MAKE_EXCEPTION("reading too much");
            size_t read_size = std::min(std::min(buffer.size(), 30ull), src.size() - total_read);
            std::transform(src.begin() + total_read, src.begin() + total_read + read_size, buffer.begin(),
                           [](char c) { return static_cast<std::byte>(c); });
            total_read += read_size;
            return read_size;
        });

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
