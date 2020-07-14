#pragma once

#include <string>
#include <vector>

#include "http/header.h"
#include "http/method.h"

namespace algue::http {

struct Request {
    Method method;
    std::string path;
    std::vector<Header> headers;
};

}  // namespace algue::http
