#pragma once

#include <cstdint>

#include <kae/flag.h>

#include "utils/bytes.h"

namespace algue::http {

enum class SettingsFrameFlags : uint8_t {
    ack = 0x1,
};

enum class HeadersFrameFlags : uint8_t {
    end_stream = 0x1,
    end_headers = 0x4,
    // padded = 0x8,
    // priority = 0x20,
};

void append_settings_frame(utils::Bytes& data, SettingsFrameFlags flags, int payload_size);
void append_headers_frame(utils::Bytes& data, int stream, HeadersFrameFlags flags, int payload_size);

}  // namespace algue::http


namespace kae {

template <>
struct IsFlag<algue::http::SettingsFrameFlags> : std::true_type {};

template <>
struct IsFlag<algue::http::HeadersFrameFlags> : std::true_type {};

}  // namespace kae
