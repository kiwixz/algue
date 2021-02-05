#pragma once

#include <string>
#include <vector>

#include "http/header.h"

namespace algue::lcu_api_explorer {

struct Lockfile {
    int port;
    std::string token;
};

Lockfile read_lockfile();
std::vector<http::Header> lockfile_to_headers(const Lockfile& lockfile);

}  // namespace algue::lcu_api_explorer
