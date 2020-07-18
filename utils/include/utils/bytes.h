#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <kae/span.h>

#include "utils/index_span.h"

namespace algue::utils {

struct BytesWriter {
    BytesWriter(kae::Span<std::byte> data);

    bool finished() const;
    size_t written() const;

    kae::Span<std::byte> put_zero(size_t n);
    void put(kae::Span<const std::byte> data);
    void put(kae::Span<const char> data);
    void put(kae::Span<const unsigned char> data);
    void put(std::string_view data);
    void put(const std::string& data);
    void put(const char* data);

    template <typename T, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, int> = 0>
    void put(T data)
    {
        return put(kae::Span{reinterpret_cast<const std::byte*>(&data), sizeof(data)});
    }

private:
    kae::Span<std::byte> data_;
    size_t index_ = 0;
};


struct Bytes : std::vector<std::byte> {
    IndexSpan<Bytes> append_zero(size_t n);
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
