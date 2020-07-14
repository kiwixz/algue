#include "http/connection.h"

#include "http/frame.h"

namespace algue::http {

utils::Bytes Connection::preface()
{
    utils::Bytes data;
    data.append("PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n");
    append_settings_frame(data, {}, 0);
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

    append_headers_frame(data, 1, HeadersFrameFlags::end_stream | HeadersFrameFlags::end_headers, 0);

    logger.hexdump(kae::LogLevel::debug, data, "request");
    return data;
}

Response Connection::parse(kae::Span<const std::byte> data)
{
    return {};
}

}  // namespace algue::http
