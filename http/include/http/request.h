#pragma once

#include <string>
#include <vector>

#include "http/header.h"

namespace algue::http {

struct Request {
    std::string method;
    std::string path;
    std::vector<Header> headers;
};

}  // namespace algue::http
