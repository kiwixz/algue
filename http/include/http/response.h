#pragma once

#include <vector>

#include "http/header.h"
#include "http/request.h"

namespace algue::http {

struct Response {
    Response() = default;
    Response(Request _request) :
        request{_request}
    {}

    Request request;
    std::vector<Header> headers;
    std::vector<std::byte> data;
};

}  // namespace algue::http
