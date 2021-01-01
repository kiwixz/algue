#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "http/header.h"
#include "utils/bytes.h"

namespace algue::http {

struct Request {
    std::string method;
    std::string path;
    std::vector<Header> headers;
    utils::Bytes body;

    bool has_header(std::string_view name) const;
    utils::Bytes serialize();
};

}  // namespace algue::http
