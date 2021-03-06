#include "http/parser.h"

#include <algorithm>
#include <charconv>
#include <iterator>
#include <type_traits>

#include <kae/exception.h>
#include <kae/pp.h>

#include "http/header_fields.h"
#include "http/methods.h"
#include "utils/bytes.h"
#include "utils/iequal.h"
#include "utils/parsing_context.h"

namespace algue::http {

Parser::Parser(MessageType type) :
    type_{type}
{}

bool Parser::finished() const
{
    return step_ == Step::finished;
}

size_t Parser::input(std::span<const std::byte> data)
{
    utils::ParsingContext ctx{{reinterpret_cast<const char*>(data.data()), data.size()}};

    switch (step_) {
    case Step::first_line: {
        std::optional<std::string_view> first_line = ctx.try_consume_until("\r\n");
        if (!first_line)
            return data.size() - ctx.remaining().size();

        parse_first_line(*first_line);
        step_ = Step::headers;
    }
        [[fallthrough]];

    case Step::headers: {
        std::vector<Header>* headers;
        switch (type_) {
        case MessageType::request:
            headers = &std::get<Request>(result_).headers;
            break;
        case MessageType::response:
            headers = &std::get<Response>(result_).headers;
            break;
        }

        while (!ctx.try_consume("\r\n")) {
            std::optional<std::string_view> header_line = ctx.try_consume_until("\r\n");
            if (!header_line)
                return data.size() - ctx.remaining().size();

            Header h;
            size_t i = header_line->find(':');
            if (i == std::string::npos)
                throw MAKE_EXCEPTION("malformed header line");

            h.name = header_line->substr(0, i);

            // trim optional whitespace
            i = std::min(header_line->find_first_not_of(" \t", i + 1), header_line->size());
            size_t end = std::min(header_line->find_last_not_of(" \t"), header_line->size());

            h.value = header_line->substr(i, end);

            if (utils::iequal(h.name, header_fields::transfer_encoding)) {
                chunked_ = (h.value.find("chunked") != std::string::npos);
            }
            else if (utils::iequal(h.name, header_fields::content_length)) {
                if (chunk_remaining_size_ != 0)
                    throw MAKE_EXCEPTION("received multiple content-length headers");

                if (std::from_chars(h.value.data(), h.value.data() + h.value.size(), chunk_remaining_size_).ptr
                    != h.value.data() + h.value.size())
                    throw MAKE_EXCEPTION("invalid content-length '{}'", h.value);
            }

            headers->push_back(std::move(h));
        }
        step_ = Step::body;
    }
        [[fallthrough]];

    case Step::body: {
        utils::Bytes* body;
        switch (type_) {
        case MessageType::request:
            body = &std::get<Request>(result_).body;
            break;
        case MessageType::response:
            body = &std::get<Response>(result_).body;
            break;
        }


        if (chunked_) {
            while (true) {
                if (chunk_remaining_size_ == 0) {  // not in the middle of a chunk, read next header
                    std::optional<std::string_view> chunk_header = ctx.try_consume_until("\r\n");
                    if (!chunk_header)
                        return data.size() - ctx.remaining().size();

                    if (std::from_chars(chunk_header->data(), chunk_header->data() + chunk_header->size(), chunk_remaining_size_, 16).ptr
                        != chunk_header->data() + chunk_header->size())
                        throw MAKE_EXCEPTION("invalid chunk size '{}'", *chunk_header);

                    if (chunk_remaining_size_ == 0) {  // empty chunk is the end marker
                        if (!ctx.try_consume("\r\n"))
                            throw MAKE_EXCEPTION("empty chunk is not empty");
                        break;
                    }
                }

                if (ctx.ended())
                    return data.size();

                size_t append_size = std::min(chunk_remaining_size_, ctx.remaining().size());

                // dont consume a whole chunk without the trailing sentinel,
                // as we currently only try to consume the sentinel after a copy
                if (ctx.remaining().size() - chunk_remaining_size_ < 2) {
                    --append_size;
                }

                *body += ctx.consume(append_size);
                chunk_remaining_size_ -= append_size;
                if (chunk_remaining_size_ > 0)
                    return data.size() - ctx.remaining().size();
                else if (!ctx.try_consume("\r\n"))
                    throw MAKE_EXCEPTION("chunk is longer than expected");
            }
        }
        else if (chunk_remaining_size_ > 0) {
            size_t append_size = std::min(chunk_remaining_size_, ctx.remaining().size());
            *body += ctx.consume(append_size);
            chunk_remaining_size_ -= append_size;
            if (chunk_remaining_size_ > 0)
                return data.size();
        }

        step_ = Step::finished;
        return data.size() - ctx.remaining().size();
    }

    case Step::finished:
        ASSERT(false);
    }
    ASSERT(false);
    std::terminate();
}

Request& Parser::request()
{
    ASSERT(step_ == Step::finished);
    return std::get<Request>(result_);
}

Response& Parser::response()
{
    ASSERT(step_ == Step::finished);
    return std::get<Response>(result_);
}

void Parser::parse_first_line(std::string_view first_line)
{
    utils::ParsingContext ctx{first_line};

    switch (type_) {
    case MessageType::request: {
        result_ = Request{};
        Request& r = std::get<Request>(result_);

        std::optional<std::string_view> method = ctx.try_consume_until(' ');
        if (!method)
            throw MAKE_EXCEPTION("invalid request line: could not parse method");
        r.method = *method;

        std::optional<std::string_view> path = ctx.try_consume_until(' ');
        if (!path)
            throw MAKE_EXCEPTION("invalid request line: could not parse path");
        r.path = *path;

        std::string_view version = ctx.remaining();
        if (version != "HTTP/1.1")
            throw MAKE_EXCEPTION("unsupported HTTP version: '{}'", version);
    } break;
    case MessageType::response: {
        result_ = Response{};
        Response& r = std::get<Response>(result_);

        std::optional<std::string_view> version = ctx.try_consume_until(' ');
        if (!version)
            throw MAKE_EXCEPTION("invalid status line: could not parse version");
        if (*version != "HTTP/1.1")
            throw MAKE_EXCEPTION("unsupported HTTP version: '{}'", *version);

        std::optional<std::string_view> status_code_str = ctx.try_consume_until(' ');
        if (!status_code_str)
            throw MAKE_EXCEPTION("invalid request line: could not parse status code");
        if (std::from_chars(status_code_str->data(), status_code_str->data() + status_code_str->size(), r.status_code).ptr
            != status_code_str->data() + status_code_str->size())
            throw MAKE_EXCEPTION("invalid status code '{}'", *status_code_str);

        r.status_message = ctx.remaining();
    } break;
    }
}

}  // namespace algue::http
