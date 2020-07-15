#include "http/frame.h"

#include <algorithm>

#include "utils/bytes.h"
#include "utils/endian.h"

namespace algue::http {
namespace {

enum class FrameType : uint8_t {
    data = 0x0,
    headers = 0x1,
    settings = 0x4,
};

void set_frame(kae::Span<std::byte> data, FrameType type, int stream, uint8_t flags, int payload_size)
{
    utils::BytesWriter writer{data};

    int payload_size_be = utils::host_to_big(payload_size);
    writer.put({reinterpret_cast<const std::byte*>(&payload_size_be) + 1, 3});
    writer.put(static_cast<uint8_t>(type));
    writer.put(flags);
    writer.put(utils::host_to_big(stream));
}

}  // namespace


void set_settings_frame(kae::Span<std::byte> data, SettingsFrameFlags flags, int payload_size)
{
    set_frame(data, FrameType::settings, 0, static_cast<uint8_t>(flags), payload_size);
}

void set_headers_frame(kae::Span<std::byte> data, int stream, HeadersFrameFlags flags, int payload_size)
{
    set_frame(data, FrameType::headers, stream, static_cast<uint8_t>(flags), payload_size);
}

}  // namespace algue::http
