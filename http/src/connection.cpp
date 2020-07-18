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
    return data;
}

utils::Bytes Connection::request(Request request, kae::UniqueFunction<void(Response)> callback)
{
    utils::Bytes data;

    utils::IndexSpan headers_frame_header = data.append_zero(HeadersFrameHeader::size);
    request.headers.insert(request.headers.begin(), {{Header::name_index(":scheme"), "https"},
                                                     {Header::name_index(":method"), std::string{to_string(request.method)}},
                                                     {Header::name_index(":path"), request.path}});
    for (const Header& hdr : request.headers) {
        hdr.encode(data.append_zero(hdr.encoded_size()));
    }
    int payload_size = static_cast<int>(data.size()) - HeadersFrameHeader::size;
    HeadersFrameHeader{next_stream_,
                       HeadersFrameHeader::Flags::end_stream | HeadersFrameHeader::Flags::end_headers,
                       payload_size}
            .encode(headers_frame_header);

    pending_[next_stream_] = {{std::move(request)}, std::move(callback)};
    ++next_stream_;
    return data;
}

void Connection::parse(kae::Span<const std::byte> data)
{
    if (!in_buffer_.empty()) {
        in_buffer_.append(data);
        data = in_buffer_;
    }

    kae::Span<const std::byte> rem = data;
    while (consume_frame(rem)) {
    }
}

bool Connection::consume_frame(kae::Span<const std::byte>& src)
{
    if (src.size() < frame_header_size)
        return false;

    int frame_size = decode_frame_size(src);
    if (static_cast<int>(src.size()) < frame_size)
        return false;

    kae::Span<const std::byte> frame_payload = src.subspan(frame_header_size, frame_size - frame_header_size);

    int frame_type_n = decode_frame_type(src);
    auto frame_type = static_cast<FrameType>(frame_type_n);
    switch (frame_type) {
    case FrameType::data: {
        logger(kae::LogLevel::info, "received data");
        DataFrameHeader header;
        header.decode(src.subspan(0, DataFrameHeader::size));

        auto it = pending_.find(header.stream);
        if (it == pending_.end())
            throw std::runtime_error{fmt::format("invalid stream {} in header frame", header.stream)};
        Pending& p = it->second;
        p.response.data.insert(p.response.data.end(), frame_payload.begin(), frame_payload.end());

        if (header.flags & DataFrameHeader::Flags::end_stream) {
            p.callback(p.response);
            pending_.erase(it);
        }
        break;
    }
    case FrameType::headers: {
        logger(kae::LogLevel::info, "received headers");
        HeadersFrameHeader header;
        header.decode(src.subspan(0, HeadersFrameHeader::size));
        assert(header.flags & HeadersFrameHeader::Flags::end_headers);  // not implemented
        assert(!(header.flags & HeadersFrameHeader::Flags::padded));    // not implemented
        assert(!(header.flags & HeadersFrameHeader::Flags::priority));  // not implemented

        auto it = pending_.find(header.stream);
        if (it == pending_.end())
            throw std::runtime_error{fmt::format("invalid stream {} in header frame", header.stream)};
        Pending& p = it->second;

        while (frame_payload.size()) {
            frame_payload = p.response.headers.emplace_back().decode(frame_payload);
        }

        if (header.flags & HeadersFrameHeader::Flags::end_stream) {
            p.callback(p.response);
            pending_.erase(it);
        }
        break;
    }
    case FrameType::settings:
        logger(kae::LogLevel::warning, "received settings, this is unimplemented");
        break;
    case FrameType::window_update:
        logger(kae::LogLevel::warning, "received window_update, this is unimplemented");
        break;
    default:
        logger(kae::LogLevel::warning, "received frame of unknown type {}", frame_type_n);
    }

    src = src.subspan(frame_size, src.size() - frame_size);
    return true;
}

}  // namespace algue::http
