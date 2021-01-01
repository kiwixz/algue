#include "http/request.h"

#include <algorithm>
#include <charconv>
#include <iterator>
#include <type_traits>

#include <kae/exception.h>
#include <kae/pp.h>

#include "http/header_fields.h"
#include "http/methods.h"
#include "utils/iequal.h"
#include "utils/parsing_context.h"

namespace algue::http {

bool Request::has_header(std::string_view name) const
{
    return std::any_of(headers.begin(), headers.end(),
                       [&](const Header& h) { return utils::iequal(h.name, name); });
}

utils::Bytes Request::serialize()
{
    ASSERT(!has_header(header_fields::transfer_encoding));
    ASSERT(has_header(header_fields::host));

    if (!body.empty()) {
        auto it = std::find_if(headers.begin(), headers.end(), [](const Header& h) {
            return utils::iequal(h.name, header_fields::content_length);
        });
        if (it == headers.end()) {
            headers.push_back({header_fields::content_length, fmt::format("{}", body.size())});
        }
        else {
            it->value = fmt::format("{}", body.size());
        }
    }

    utils::Bytes r;

    r += method;
    r += " ";
    r += path;
    r += " HTTP/1.1\r\n";

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
