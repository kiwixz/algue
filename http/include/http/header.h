#pragma once

#include <string>

#include <kae/span.h>

namespace algue::http {

struct Header {
    std::string name;
    std::string value;

    size_t encoded_size() const;
    void encode(kae::Span<std::byte> dest) const;
    void decode(kae::Span<const std::byte> src);
};

}  // namespace algue::http
