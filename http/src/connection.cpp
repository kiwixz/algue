#include "http/connection.h"

#include <cassert>

#include "http/frame.h"
#include "utils/endian.h"

namespace algue::http {

utils::Bytes Connection::preface()
{
    utils::Bytes data;
    data.append("PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n");
    set_settings_frame(data.append_zero(settings_frame_size), {}, 0);

    logger.hexdump(kae::LogLevel::debug, data, "preface");
    return data;
}

utils::Bytes Connection::request(Request request, kae::UniqueFunction<void(Response)> callback)
{
    utils::Bytes data;
    utils::IndexSpan header_header = data.append_zero(headers_frame_size);
    append_header(data, ":scheme", "https");
    append_header(data, ":method", to_string(request.method));
    append_header(data, ":path", request.path);
    for (const Header& header : request.headers) {
        append_header(data, header.name, header.value);
    }
    int payload_size = static_cast<int>(data.size()) - headers_frame_size;
    set_headers_frame(header_header, next_stream_, HeadersFrameFlags::end_stream | HeadersFrameFlags::end_headers, payload_size);

    pending_[next_stream_] = {{std::move(request)}, std::move(callback)};
    ++next_stream_;

    logger.hexdump(kae::LogLevel::debug, data, "request");
    return data;
}

void Connection::parse(kae::Span<const std::byte> data)
{
    logger.hexdump(kae::LogLevel::debug, data, "parse");
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

}  // namespace algue::http
