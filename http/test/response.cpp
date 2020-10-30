#include "http/response.h"

#include <algorithm>
#include <iostream>
#include <string_view>
#include <vector>

#include <doctest/doctest.h>

#include "http/header_fields.h"
#include "http/status.h"

namespace algue::http::test {

TEST_SUITE("response")
{
    TEST_CASE("serialize")
    {
        Response res;
        res.status_code = 299;
        res.status_message = "nice";
        res.headers.push_back({header_fields::host, "some.host.name.com"});
        res.headers.push_back({header_fields::accept, "application/json"});
        res.headers.push_back({"Custom-Header", "custom value"});

        std::string_view body = "hello world!";
        std::transform(body.begin(), body.end(), std::back_inserter(res.body),
                       [](char c) { return static_cast<std::byte>(c); });

        std::vector<std::byte> v = res.serialize();
        CHECK(std::string_view{reinterpret_cast<const char*>(v.data()), v.size()}
              == "HTTP/1.1 299 nice\r\n"
                 "host: some.host.name.com\r\n"
                 "accept: application/json\r\n"
                 "Custom-Header: custom value\r\n"
                 "content-length: 12\r\n"
                 "\r\n"
                 "hello world!");
    }
}

}  // namespace algue::http::test
