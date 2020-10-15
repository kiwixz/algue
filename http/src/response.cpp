#include "http/response.h"

#include <algorithm>
#include <charconv>

#include <kae/exception.h>
#include <kae/logger.h>

#include "http/header_fields.h"

namespace algue::http {

void Response::deserialize(kae::FunctionRef<size_t(std::span<std::byte> buffer)> read)
{
    size_t block_size = 4096;
    std::string_view header_sentinel = "\r\n\r\n";

    std::vector<std::byte> buffer;
    buffer.resize(block_size);

    size_t last_read_size = 0;
    size_t total_read_size = 0;

    std::string_view header;

    // read until we have the full header
    while (true) {
        size_t available_size = buffer.size() - total_read_size;
        if (available_size < block_size) {
            buffer.resize(buffer.size() * 2);
            available_size = buffer.size() - total_read_size;
        }
        last_read_size = read({buffer.data() + total_read_size, available_size});
        ASSERT(last_read_size > 0);
        total_read_size += last_read_size;

        auto it = std::search(buffer.begin() + total_read_size - last_read_size,
                              buffer.begin() + total_read_size,
                              reinterpret_cast<const std::byte*>(header_sentinel.data()),
                              reinterpret_cast<const std::byte*>(header_sentinel.data()) + header_sentinel.size());
        if (it != buffer.begin() + total_read_size) {
            header = {reinterpret_cast<const char*>(buffer.data()),
                      (it - buffer.begin()) + header_sentinel.size()};
            break;
        }
    }

    std::string_view rem = header;
    auto consume = [&](std::string_view sentinel) {
        size_t i = rem.find(sentinel);
        if (i == std::string_view::npos)
            throw MAKE_EXCEPTION("could not find expected '{}'", sentinel);

        std::string_view r = rem.substr(0, i);
        rem = rem.substr(i + sentinel.size());
        return r;
    };

    std::string_view version = consume(" ");
    if (version != "HTTP/1.1")
        throw MAKE_EXCEPTION("invalid version '{}'", version);

    std::string_view status_code_str = consume(" ");
    if (std::from_chars(status_code_str.data(), status_code_str.data() + status_code_str.size(), status_code).ptr
        != status_code_str.data() + status_code_str.size())
        throw MAKE_EXCEPTION("invalid status code '{}'", status_code_str);

    status_message = consume("\r\n");

    headers.clear();
    while (rem.substr(0, 2) != "\r\n") {
        Header h;

        h.name = consume(":");
        rem.remove_prefix(std::min(rem.find_first_not_of(" \t"), rem.size()));

        std::string_view value = consume("\r\n");
        value.remove_suffix(std::find_if_not(value.rbegin(), value.rend(),
                                             [](char c) { return c == ' ' || c == '\t'; })
                            - value.rbegin());
        h.value = value;

        headers.push_back(std::move(h));
    }

    if (std::any_of(headers.begin(), headers.end(),
                    [](const Header& h) { return h.name == header_fields::transfer_encoding; }))
        throw MAKE_EXCEPTION("transfer-encoding is not implementeds");

    auto it = std::find_if(headers.begin(), headers.end(),
                           [](const Header& h) { return h.name == header_fields::content_length; });
    if (it == headers.end())  // no body
        return;

    size_t body_size;
    if (std::from_chars(it->value.data(), it->value.data() + it->value.size(), body_size).ptr
        != it->value.data() + it->value.size())
        throw MAKE_EXCEPTION("invalid content-length '{}'", it->value);

    body.resize(body_size);
    size_t body_read_size = total_read_size - header.size();
    std::copy(buffer.begin() + header.size(), buffer.begin() + total_read_size, body.begin());
    while (body_read_size < body_size) {
        body_read_size += read({body.data() + body_read_size, body_size - body_read_size});
    }
}

}  // namespace algue::http
