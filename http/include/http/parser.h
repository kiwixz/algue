#pragma once

#include <span>
#include <string_view>
#include <variant>

#include "http/request.h"
#include "http/response.h"

namespace algue::http {

enum class MessageType {
    request,
    response,
};


struct Parser {
    Parser(MessageType type);

    bool finished() const;

    /// Returns the number of "unparsed" bytes to keep.
    size_t input(std::span<const std::byte> data);

    Request& request();
    Response& response();

private:
    enum class Step {
        first_line,
        headers,
        body,
        finished,
    };

    MessageType type_;
    std::variant<Request, Response> result_;
    Step step_ = Step::first_line;
    bool chunked_ = false;
    size_t chunk_remaining_size_ = 0;

    void parse_first_line(std::string_view first_line);
};

}  // namespace algue::http
