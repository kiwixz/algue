#pragma once

#include <algorithm>
#include <bit>
#include <string_view>
#include <vector>

namespace algue::utils {

struct Bytes {
    template <typename T>
    Bytes& operator+=(const T& container)
    {
        size_t old_size = data_.size();
        size_t new_size = old_size + container.size();
        data_.reserve(std::bit_ceil(new_size));
        data_.resize(new_size);
        if constexpr (std::is_same_v<std::decay_t<decltype(*container.begin())>, char>) {
            std::transform(container.begin(), container.end(), data_.begin() + old_size,
                           [](auto b) { return static_cast<std::byte>(b); });
        }
        else {
            std::copy(container.begin(), container.end(), data_.begin() + old_size);
        }

        return *this;
    }

    Bytes& operator+=(const char* cstr)
    {
        return *this += std::string_view{cstr};
    }

    std::byte* begin()
    {
        return data_.data();
    }
    const std::byte* begin() const
    {
        return data_.data();
    }

    std::byte* end()
    {
        return data_.data() + data_.size();
    }
    const std::byte* end() const
    {
        return data_.data() + data_.size();
    }

    bool empty() const
    {
        return data_.empty();
    }

    size_t size() const
    {
        return data_.size();
    }

    std::string_view as_chars() const
    {
        return {reinterpret_cast<const char*>(data_.data()), data_.size()};
    }

    std::byte* data()
    {
        return data_.data();
    }
    const std::byte* data() const
    {
        return data_.data();
    }

    void reserve(size_t new_size)
    {
        data_.reserve(new_size);
    }

    void resize(size_t new_size)
    {
        data_.resize(new_size);
    }

private:
    std::vector<std::byte> data_;
};

}  // namespace algue::utils
