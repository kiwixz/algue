#include "http/request.h"

#include <iostream>
#include <string_view>
#include <vector>

#include <doctest/doctest.h>

#include "http/header_fields.h"
#include "http/methods.h"

namespace algue::http::test {

TEST_SUITE("request")
{
    TEST_CASE("serialize")
    {
        Request req;
        req.method = methods::post;
        req.path = "/some/file%20name.html?with=parameter&and=another_one";
        req.headers.push_back({header_fields::host, "some.host.name.com"});
        req.headers.push_back({header_fields::accept, "application/json"});
        req.headers.push_back({"Custom-Header", "custom value"});

        utils::Bytes v = req.serialize();
        CHECK(v.as_chars() == "POST /some/file%20name.html?with=parameter&and=another_one HTTP/1.1\r\n"
                              "host: some.host.name.com\r\n"
                              "accept: application/json\r\n"
                              "Custom-Header: custom value\r\n"
                              "\r\n");
    }
}

}  // namespace algue::http::test
