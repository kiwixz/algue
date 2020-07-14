#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <kae/span.h>

namespace algue::utils {

struct Bytes : std::vector<std::byte> {
    iterator append(kae::Span<const std::byte> data);
    iterator append(kae::Span<const char> data);
    iterator append(kae::Span<const unsigned char> data);
    iterator append(std::string_view data);
    iterator append(const std::string& data);
    iterator append(const char* data);

    template <typename T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, int> = 0>
    iterator append(T data)
    {
        return append(kae::Span{reinterpret_cast<const std::byte*>(&data), sizeof(data)});
    }
};

}  // namespace algue::utils
