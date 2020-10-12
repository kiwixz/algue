#pragma once

#include <vector>

#include <kae/function_ref.h>
#include <kae/span.h>

#include "http/header.h"
#include "http/request.h"

namespace algue::http {

struct Response {
    Request request;

    int status_code;
    std::string status_message;
    std::vector<Header> headers;
    std::vector<std::byte> body;

    void deserialize(kae::FunctionRef<size_t(kae::Span<std::byte> buffer)> read);
};

}  // namespace algue::http
