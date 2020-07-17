#pragma once

#include <string>
#include <string_view>

#include <kae/span.h>

namespace algue::http {

size_t huffman_size(std::string_view src);
void huffman_encode(std::string_view src, kae::Span<std::byte> dest);
std::string huffman_decode(kae::Span<const std::byte> src);

}  // namespace algue::http
