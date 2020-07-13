#pragma once

#include <string>

namespace algue::utils {

struct Lockfile {
    int port;
    std::string token;
};

Lockfile read_lockfile();

}  // namespace algue::utils
