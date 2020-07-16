#pragma once

#include "http/request.h"

namespace algue::http {

struct Response {
    Response() = default;
    Response(Request _request) :
        request{_request}
    {}

    Request request;
    std::vector<Header> headers;
};

}  // namespace algue::http
