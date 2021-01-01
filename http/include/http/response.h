#pragma once

#include <span>
#include <vector>

#include <kae/function_ref.h>

#include "http/header.h"
#include "utils/bytes.h"

namespace algue::http {

struct Response {
    int status_code;
    std::string status_message;
    std::vector<Header> headers;
    utils::Bytes body;

    bool has_header(std::string_view name) const;
    utils::Bytes serialize();
};

}  // namespace algue::http
