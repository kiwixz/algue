#include "utils/self_path.h"

#include <array>

#ifdef _WIN32
#    define WIN32_LEAN_AND_MEAN
#    define NOMINMAX
#    include <Windows.h>
#endif

namespace algue::utils {

std::filesystem::path get_self_path()
{
#ifdef _WIN32
    int block_size = 4096;

    std::string buf;
    size_t path_size = 0;
    while (path_size == buf.size()) {
        buf.resize(buf.size() + block_size);
        buf.resize(GetModuleFileName(nullptr, buf.data(), buf.size()));
        if (buf.empty())
            throw std::runtime_error{"could not get self path"};
    }

    return buf;
#else
    return std::filesystem::read_symlink("/proc/self/exe");
#endif
}

}  // namespace algue::utils
