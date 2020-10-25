#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "http/header.h"

namespace algue::http {

struct Request {
    std::string method;
    std::string path;
    std::vector<Header> headers;
    std::vector<std::byte> body;

    bool has_header(std::string_view name) const;
    std::vector<std::byte> serialize();
};


struct RequestParser {
    /// Returns the number of "unparsed" bytes to keep.
    size_t input(std::span<const std::byte> data);

    /// If parsing is finished, return a move of the request.
    std::optional<Request> get();

private:
    enum class Step {
        method,
        path,
        version,
        headers,
        body,
        finished,
    };

    Request request_;
    Step step_ = Step::method;
    size_t body_read_ = 0;
};

}  // namespace algue::http
