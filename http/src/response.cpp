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

utils::Bytes Response::serialize()
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

    utils::Bytes r;

    r += "HTTP/1.1 ";
    r += fmt::format("{}", status_code);
    r += " ";
    r += status_message;
    r += "\r\n";

    for (const Header& h : headers) {
        r += h.name;
        r += ": ";
        r += h.value;
        r += "\r\n";
    }
    r += "\r\n";

    r += body;
    return r;
}

}  // namespace algue::http
