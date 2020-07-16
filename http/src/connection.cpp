#include "http/connection.h"

#include <cassert>

#include "http/frame.h"
#include "utils/endian.h"

namespace algue::http {

utils::Bytes Connection::preface()
{
    utils::Bytes data;
    data.append("PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n");
    SettingsFrameHeader{}.encode(data.append_zero(SettingsFrameHeader::size));

    logger.hexdump(kae::LogLevel::debug, data, "preface");
    return data;
}

utils::Bytes Connection::request(Request request, kae::UniqueFunction<void(Response)> callback)
{
    utils::Bytes data;
    utils::IndexSpan headers_frame_header = data.append_zero(HeadersFrameHeader::size);
    append_header(data, ":scheme", "https");
    append_header(data, ":method", to_string(request.method));
    append_header(data, ":path", request.path);
    for (const Header& header : request.headers) {
        append_header(data, header.name, header.value);
    }
    int payload_size = static_cast<int>(data.size()) - HeadersFrameHeader::size;
    HeadersFrameHeader{next_stream_,
                       HeadersFrameHeader::Flags::end_stream | HeadersFrameHeader::Flags::end_headers,
                       payload_size}
            .encode(headers_frame_header);

    pending_[next_stream_] = {{std::move(request)}, std::move(callback)};
    ++next_stream_;

    logger.hexdump(kae::LogLevel::debug, data, "request");
    return data;
}

void Connection::parse(kae::Span<const std::byte> data)
{
    logger.hexdump(kae::LogLevel::debug, data, "parse");

    if (!in_buffer_.empty()) {
        in_buffer_.append(data);
        data = in_buffer_;
    }

    kae::Span<const std::byte> rem = data;
    while (consume_frame(rem)) {
    }
}

void Connection::append_header(utils::Bytes& data, std::string_view name, std::string_view value)
{
    // compression is not implemented yet
    // neither is integer encoding
    assert(name.size() < 128);
    assert(value.size() < 128);

    assert(!std::any_of(name.begin(), name.end(), [&](char c) { return c >= 'A' && c <= 'Z'; }));

    data.append(uint8_t{0});
    data.append(utils::host_to_big(static_cast<uint8_t>(name.size())));
    data.append(name);
    data.append(utils::host_to_big(static_cast<uint8_t>(value.size())));
    data.append(value);
}

bool Connection::consume_frame(kae::Span<const std::byte>& src)
{
    if (src.size() < frame_header_size)
        return false;

    int frame_size = decode_frame_size(src);
    if (static_cast<int>(src.size()) < frame_size)
        return false;

    logger.hexdump(kae::LogLevel::debug, src.subspan(0, frame_size), "frame");

    int frame_type_n = decode_frame_type(src);
    FrameType frame_type = static_cast<FrameType>(frame_type_n);
    switch (frame_type) {
    case FrameType::data:
        logger(kae::LogLevel::warning, "received data, this is unimplemented");
        break;
    case FrameType::headers:
        logger(kae::LogLevel::warning, "received headers, this is unimplemented");
        break;
    case FrameType::settings:
        logger(kae::LogLevel::warning, "received settings, this is unimplemented");
        break;
    default:
        logger(kae::LogLevel::warning, "received frame of unknown type {}", frame_type_n);
    }

    src = src.subspan(frame_size, src.size() - frame_size);
    return true;
}

}  // namespace algue::http
