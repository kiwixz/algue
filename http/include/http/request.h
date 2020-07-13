#pragma once

#include <string>
#include <vector>

#include "http/method.h"
#include "http/header.h"

namespace algue::http {

struct Request {
    Method method;
    std::string path;
    std::vector<Header> headers;
};

}  // namespace algue::http
