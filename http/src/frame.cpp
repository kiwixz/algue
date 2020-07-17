#include "http/frame.h"

#include <cassert>

#include <kae/bit_cast.h>

#include "utils/bytes.h"
#include "utils/endian.h"

namespace algue::http {
namespace {

void set_frame_header(kae::Span<std::byte> dest, FrameType type, int stream, uint8_t flags, int payload_size)
{
    utils::BytesWriter writer{dest};

    int payload_size_be = utils::host_to_big(payload_size);
    writer.put({reinterpret_cast<const std::byte*>(&payload_size_be) + 1, 3});
    writer.put(static_cast<uint8_t>(type));
    writer.put(flags);
    writer.put(utils::host_to_big(stream));
}

uint8_t decode_frame_flags(kae::Span<const std::byte> src)
{
    return kae::bit_cast<uint8_t>(src[4]);
}

Stream decode_frame_stream(kae::Span<const std::byte> src)
{
    Stream r;
    std::copy(src.begin() + 5, src.begin() + 5 + sizeof(Stream),
              reinterpret_cast<std::byte*>(&r));
    return utils::big_to_host(r);
}

}  // namespace


int decode_frame_size(kae::Span<const std::byte> src)
{
    assert(src.size() >= frame_header_size);
    int payload_size_be = 0;
    std::copy(src.data(), src.data() + 3, reinterpret_cast<std::byte*>(&payload_size_be) + 1);
    return frame_header_size + utils::big_to_host(payload_size_be);
}

int decode_frame_type(kae::Span<const std::byte> src)
{
    assert(src.size() >= frame_header_size);
    return kae::bit_cast<uint8_t>(src[3]);
}


void SettingsFrameHeader::encode(kae::Span<std::byte> dest)
{
    assert(dest.size() == size);
    set_frame_header(dest, FrameType::settings,
                     0, static_cast<uint8_t>(flags), payload_size);
}

void SettingsFrameHeader::decode(kae::Span<const std::byte> src)
{
    assert(src.size() == size);
    flags = static_cast<SettingsFrameHeader::Flags>(decode_frame_flags(src));
    payload_size = decode_frame_size(src);
}


void HeadersFrameHeader::encode(kae::Span<std::byte> dest)
{
    assert(dest.size() == size);
    set_frame_header(dest, FrameType::headers,
                     stream, static_cast<uint8_t>(flags), payload_size);
}

void HeadersFrameHeader::decode(kae::Span<const std::byte> src)
{
    assert(src.size() == size);
    stream = decode_frame_stream(src);
    flags = static_cast<HeadersFrameHeader::Flags>(decode_frame_flags(src));
    payload_size = decode_frame_size(src);
}

}  // namespace algue::http
