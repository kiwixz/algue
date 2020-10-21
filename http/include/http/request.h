#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "http/header.h"

namespace algue::http {

struct Request {
    std::string method;
    std::string path;
    std::vector<Header> headers;
    std::vector<std::byte> body;

    bool has_header(std::string_view name) const;
    std::vector<std::byte> serialize();
};

}  // namespace algue::http
