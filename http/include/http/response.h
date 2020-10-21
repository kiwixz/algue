#pragma once

#include <span>
#include <vector>

#include <kae/function_ref.h>

#include "http/header.h"

namespace algue::http {

struct Response {
    int status_code;
    std::string status_message;
    std::vector<Header> headers;
    std::vector<std::byte> body;

    void deserialize(kae::FunctionRef<size_t(std::span<std::byte> buffer)> read);
};

}  // namespace algue::http
