#pragma once

#include <string>

namespace algue::http {

struct Header {
    std::string name;
    std::string value;
};

}  // namespace algue::http
