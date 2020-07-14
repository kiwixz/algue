#include "http/frame.h"

#include "utils/endian.h"

namespace algue::http {
namespace {

enum class FrameType : uint8_t {
    data = 0x0,
    headers = 0x1,
    settings = 0x4,
};

void append_frame(utils::Bytes& data, FrameType type, int stream, uint8_t flags, int payload_size)
{
    int payload_size_be = utils::host_to_big(payload_size);
    data.append({reinterpret_cast<const std::byte*>(&payload_size_be), 3});
    data.append(static_cast<uint8_t>(type));
    data.append(flags);
    data.append(utils::host_to_big(stream));
}

}  // namespace


void append_settings_frame(utils::Bytes& data, SettingsFrameFlags flags, int payload_size)
{
    append_frame(data, FrameType::settings, 0, static_cast<uint8_t>(flags), payload_size);
}

void append_headers_frame(utils::Bytes& data, int stream, HeadersFrameFlags flags, int payload_size)
{
    append_frame(data, FrameType::headers, stream, static_cast<uint8_t>(flags), payload_size);
}

}  // namespace algue::http
