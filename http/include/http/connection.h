#pragma once

#include <unordered_map>

#include <kae/function.h>
#include <kae/logger.h>
#include <kae/span.h>

#include "http/frame.h"
#include "http/request.h"
#include "http/response.h"
#include "utils/bytes.h"

namespace algue::http {

struct Connection {
    utils::Bytes preface();
    utils::Bytes request(Request request, kae::UniqueFunction<void(Response)> callback);

    void parse(kae::Span<const std::byte> data);

private:
    struct Pending {
        Response response;
        kae::UniqueFunction<void(Response)> callback;
    };

    kae::Logger logger{"Connection"};
    Stream next_stream_ = 1;
    std::unordered_map<Stream, Pending> pending_;
    utils::Bytes in_buffer_;

    bool consume_frame(kae::Span<const std::byte>& src);
};

}  // namespace algue::http
