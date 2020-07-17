#pragma once

#include <unordered_map>

#include <kae/span.h>

namespace algue::http {

void huffman_size(std::string_view src);
void huffman_encode(std::string_view src, kae::Span<std::byte> dest);
std::string huffman_decode(kae::Span<const std::byte> src);

}  // namespace algue::http
