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
        res.body += "hello world!";

        utils::Bytes v = res.serialize();
        CHECK(v.as_chars() == "HTTP/1.1 299 nice\r\n"
                              "host: some.host.name.com\r\n"
                              "accept: application/json\r\n"
                              "Custom-Header: custom value\r\n"
                              "content-length: 12\r\n"
                              "\r\n"
                              "hello world!");
    }
}

}  // namespace algue::http::test
