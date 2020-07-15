#pragma once

#include <cstdint>

#include <kae/flag.h>
#include <kae/span.h>

namespace algue::http {

enum class SettingsFrameFlags {
    ack = 0x1,
};

enum class HeadersFrameFlags {
    end_stream = 0x1,
    end_headers = 0x4,
    // padded = 0x8,
    // priority = 0x20,
};

constexpr int frame_size = 9;
constexpr int headers_frame_size = frame_size;
constexpr int settings_frame_size = frame_size;

void set_settings_frame(kae::Span<std::byte> data, SettingsFrameFlags flags, int payload_size);
void set_headers_frame(kae::Span<std::byte> data, int stream, HeadersFrameFlags flags, int payload_size);

}  // namespace algue::http


namespace kae {

template <>
struct IsFlag<algue::http::SettingsFrameFlags> : std::true_type {};

template <>
struct IsFlag<algue::http::HeadersFrameFlags> : std::true_type {};

}  // namespace kae
