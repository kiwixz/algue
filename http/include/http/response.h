#pragma once

#include "http/request.h"

namespace algue::http {

struct Response {
    Request request;
    std::vector<Header> headers;
};

}  // namespace algue::http
