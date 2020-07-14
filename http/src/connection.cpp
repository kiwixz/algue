#include "http/connection.h"

#include "utils/endian.h"

namespace algue::http {

utils::Bytes Connection::preface()
{
    utils::Bytes data;
    data.append("PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n");
    append_frame_header(data, FrameType::settings, 0, 0, 0);
    return data;
}

utils::Bytes Connection::request(Request request)
{
    std::string_view eol = "\r\n";

    utils::Bytes data;
    data.append(to_string(request.method));
    data.append(" ");
    data.append(request.path);
    data.append(" ");
    data.append("HTTP/1.1");
    data.append(eol);
    for (const Header& header : request.headers) {
        data.append(header.name);
        data.append(": ");
        data.append(header.value);
        data.append(eol);
    }
    data.append(eol);

    logger(kae::LogLevel::debug, "making http request:\n{}",
           std::string_view{reinterpret_cast<const char*>(data.data()), data.size()});

    return data;
}

Response Connection::parse(kae::Span<const std::byte> data)
{
    return {};
}

void Connection::append_frame_header(utils::Bytes& data, FrameType type, int stream, uint8_t flags, int payload_size)
{
    int payload_size_be = utils::host_to_big(payload_size);
    data.append({reinterpret_cast<const std::byte*>(&payload_size_be), 3});
    data.append(static_cast<uint8_t>(type));
    data.append(flags);
    data.append(utils::host_to_big(stream));
}

}  // namespace algue::http
