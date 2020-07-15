#include "http/connection.h"

#include "http/frame.h"

namespace algue::http {

utils::Bytes Connection::preface()
{
    utils::Bytes data;
    data.append("PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n");
    set_settings_frame(data.append_zero(settings_frame_size), {}, 0);
    logger.hexdump(kae::LogLevel::debug, data, "preface");
    return data;
}

utils::Bytes Connection::request(Request request)
{
    utils::Bytes data;

    // constexpr std::string_view eol = "\r\n";
    // data.append(to_string(request.method));
    // data.append(" ");
    // data.append(request.path);
    // data.append(" ");
    // data.append("HTTP/1.1");
    // data.append(eol);
    // for (const Header& header : request.headers) {
    //     data.append(header.name);
    //     data.append(": ");
    //     data.append(header.value);
    //     data.append(eol);
    // }
    // data.append(eol);

    kae::Span<std::byte> header_frame = data.append_zero(headers_frame_size);
    int header_size = static_cast<int>(data.size());
    append_header(data, ":scheme", "http");
    append_header(data, ":method", to_string(request.method));
    append_header(data, ":path", request.path);
    for (const Header& header : request.headers) {
        append_header(data, header.name, header.value);
    }
    int payload_size = static_cast<int>(data.size()) - header_size;
    set_headers_frame(header_frame, 1, HeadersFrameFlags::end_stream | HeadersFrameFlags::end_headers, payload_size);

    logger.hexdump(kae::LogLevel::debug, data, "request");
    return data;
}

Response Connection::parse(kae::Span<const std::byte> data)
{
    return {};
}

void Connection::append_header(utils::Bytes& data, std::string_view name, std::string_view value)
{
    // compression is not implemented yet
    data.append(0u);
    data.append(static_cast<unsigned int>(name.size()));
    data.append(name);
    data.append(static_cast<unsigned int>(value.size()));
    data.append(value);
}

}  // namespace algue::http
