#pragma once

#include <string>
#include <vector>

#include "http/header.h"

namespace algue::utils {

struct Lockfile {
    int port;
    std::string token;
};

Lockfile read_lockfile();
std::vector<http::Header> lockfile_to_headers(const Lockfile& lockfile);

}  // namespace algue::utils
