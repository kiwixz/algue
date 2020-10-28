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

std::vector<std::byte> Request::serialize()
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

    append(method);
    append(" ");
    append(path);
    append(" HTTP/1.1\r\n");
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


size_t RequestParser::input(std::span<const std::byte> data)
{
    utils::ParsingContext ctx{{reinterpret_cast<const char*>(data.data()), data.size()}};

    switch (step_) {
    case Step::method: {
        std::optional<std::string_view> method = ctx.try_consume_until(' ');
        if (!method)
            return ctx.remaining().size();
        request_.method = *method;
        step_ = Step::path;
    }
        [[fallthrough]];

    case Step::path: {
        std::optional<std::string_view> path = ctx.try_consume_until(' ');
        if (!path)
            return ctx.remaining().size();
        request_.path = *path;
        step_ = Step::version;
    }
        [[fallthrough]];

    case Step::version: {
        std::optional<std::string_view> version = ctx.try_consume_until("\r\n");
        if (!version)
            return ctx.remaining().size();
        if (*version != "HTTP/1.1")
            throw MAKE_EXCEPTION("unsupported HTTP version: '{}'", *version);
        step_ = Step::headers;
    }
        [[fallthrough]];

    case Step::headers:
        while (!ctx.try_consume("\r\n")) {
            std::optional<std::string_view> header_line = ctx.try_consume_until("\r\n");
            if (!header_line)
                return ctx.remaining().size();

            Header h;
            size_t i = header_line->find(':');
            if (i == std::string::npos)
                throw MAKE_EXCEPTION("malformed header line");

            h.name = header_line->substr(0, i);

            // trim optional whitespace
            i = std::min(header_line->find_first_not_of(" \t", i + 1), header_line->size());
            size_t end = std::min(header_line->find_last_not_of(" \t"), header_line->size());

            h.value = header_line->substr(i, end);

            if (utils::iequal(h.name, header_fields::transfer_encoding))
                throw MAKE_EXCEPTION("transfer-encoding is not implemented");
            else if (utils::iequal(h.name, header_fields::content_length)) {
                if (!request_.body.empty())
                    throw MAKE_EXCEPTION("received multiple content-length headers");

                size_t body_size;
                if (std::from_chars(h.value.data(), h.value.data() + h.value.size(), body_size).ptr
                    != h.value.data() + h.value.size())
                    throw MAKE_EXCEPTION("invalid content-length '{}'", h.value);

                request_.body.resize(body_size);
            }

            request_.headers.push_back(std::move(h));
        }
        step_ = Step::body;
        [[fallthrough]];

    case Step::body:
        if (body_read_ < request_.body.size()) {
            size_t body_data_size = std::min(request_.body.size() - body_read_, ctx.remaining().size());
            std::span<const std::byte> body_data{reinterpret_cast<const std::byte*>(ctx.consume(body_data_size).data()), body_data_size};
            std::copy(body_data.begin(), body_data.end(), request_.body.begin() + body_read_);
            body_read_ += body_data.size();

            if (body_read_ < request_.body.size())
                return 0;
        }
        step_ = Step::finished;
        return ctx.remaining().size();

    case Step::finished:
        ASSERT(false);
    }
    ASSERT(false);
    std::terminate();
}

std::optional<Request> RequestParser::get()
{
    if (step_ == Step::finished)
        return std::move(request_);
    else
        return {};
}

}  // namespace algue::http
