#include "utils/lockfile.h"

#include <charconv>
#include <fstream>

#include <fmt/format.h>

#include "utils/base64.h"

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
    const char* port_end = file.data() + idx_token - 1;
    if (std::from_chars(file.data() + idx_port, port_end, lockfile.port).ptr != port_end)
        throw std::runtime_error{"could not parse lockfile port"};

    lockfile.token = file.substr(idx_token, end_token - idx_token - 1);
    return lockfile;
}

std::vector<http::Header> lockfile_to_headers(const Lockfile& lockfile)
{
    return {
            {"host", fmt::format("127.0.0.1:{}", lockfile.port)},
            {"authorization",
             fmt::format("Basic {}",
                         utils::base64(fmt::format("riot:{}", lockfile.token)))},
    };
}

}  // namespace algue::utils
