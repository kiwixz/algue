#include "http/response.h"

#include <algorithm>
#include <charconv>

#include <kae/exception.h>
#include <kae/logger.h>

#include "http/header_fields.h"
#include "utils/iequal.h"

namespace algue::http {

bool Response::has_header(std::string_view name) const
{
    return std::any_of(headers.begin(), headers.end(),
                       [&](const Header& h) { return utils::iequal(h.name, name); });
}

std::vector<std::byte> Response::serialize()
{
    ASSERT(!has_header(header_fields::transfer_encoding));

    auto it = std::find_if(headers.begin(), headers.end(), [](const Header& h) {
        return utils::iequal(h.name, header_fields::content_length);
    });
    if (it == headers.end()) {
        headers.push_back({header_fields::content_length, fmt::format("{}", body.size())});
    }
    else {
        it->value = fmt::format("{}", body.size());
    }

    std::vector<std::byte> r;
    r.reserve(4096);

    auto append = [&](const auto& c) {
        if constexpr (std::is_same_v<std::decay_t<decltype(c)>, const char*>) {
            std::string_view str = c;
            std::transform(str.begin(), str.end(), std::back_inserter(r),
                           [](auto b) { return static_cast<std::byte>(b); });
        }
        else {
            std::transform(c.begin(), c.end(), std::back_inserter(r),
                           [](auto b) { return static_cast<std::byte>(b); });
        }
    };

    append("HTTP/1.1 ");
    append(fmt::format("{}", status_code));
    append(" ");
    append(status_message);
    append("\r\n");
    for (const Header& h : headers) {
        append(h.name);
        append(": ");
        append(h.value);
        append("\r\n");
    }
    append("\r\n");
    append(body);

    return r;
}

}  // namespace algue::http
