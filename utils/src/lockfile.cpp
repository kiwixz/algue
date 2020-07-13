#include "utils/lockfile.h"

#include <charconv>
#include <fstream>

namespace algue::utils {

Lockfile read_lockfile()
{
    std::ifstream ifs{"C:/Riot Games/League of Legends/lockfile", std::ios::ate};
    if (!ifs)
        throw std::runtime_error{"no lockfile, is league launched?"};

    std::string file;
    file.resize(ifs.tellg());
    ifs.seekg(0, std::ios::beg);
    ifs.read(file.data(), file.size());

    auto next = [&](size_t start) {
        size_t r = file.find(':', start);
        if (r == std::string::npos)
            throw std::runtime_error{"corrupt lockfile"};
        return r + 1;
    };

    size_t idx_port = next(next(0));
    size_t idx_token = next(idx_port);
    size_t end_token = next(idx_token);

    Lockfile lockfile;
    if (std::from_chars(file.data() + idx_port, file.data() + idx_token - 1, lockfile.port).ptr != file.data() + idx_token - 1)
        throw std::runtime_error{"could not parse lockfile port"};

    lockfile.token = file.substr(idx_token, end_token - idx_token - 1);
    return lockfile;
}

}  // namespace algue::utils
