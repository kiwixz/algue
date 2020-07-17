#pragma once

#include <cstdint>

#include <kae/flag.h>
#include <kae/span.h>

namespace algue::http {

using Stream = int;

enum class FrameType {
    data = 0x0,
    headers = 0x1,
    settings = 0x4,
};

constexpr int frame_header_size = 9;

int decode_frame_size(kae::Span<const std::byte> src);
int decode_frame_type(kae::Span<const std::byte> src);


struct SettingsFrameHeader {
    enum class Flags {
        ack = 0x1,
    };

    static constexpr int size = frame_header_size;

    Flags flags;
    int payload_size;

    void encode(kae::Span<std::byte> dest);
    void decode(kae::Span<const std::byte> src);
};


struct HeadersFrameHeader {
    enum class Flags {
        end_stream = 0x1,
        end_headers = 0x4,
        padded = 0x8,
        priority = 0x20,
    };

    static constexpr int size = frame_header_size;

    Stream stream;
    Flags flags;
    int payload_size;

    void encode(kae::Span<std::byte> dest);
    void decode(kae::Span<const std::byte> src);
};

}  // namespace algue::http


namespace kae {

template <>
struct IsFlag<algue::http::SettingsFrameHeader::Flags> : std::true_type {};

template <>
struct IsFlag<algue::http::HeadersFrameHeader::Flags> : std::true_type {};

}  // namespace kae
