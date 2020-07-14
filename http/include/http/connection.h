#pragma once

#include <kae/logger.h>
#include <kae/span.h>

#include "http/frame_type.h"
#include "http/request.h"
#include "http/response.h"
#include "utils/bytes.h"

namespace algue::http {

struct Connection {
    utils::Bytes preface();
    utils::Bytes request(Request request);

    Response parse(kae::Span<const std::byte> data);

private:
    kae::Logger logger{"Connection"};

    void append_frame_header(utils::Bytes& data, FrameType type, int stream, uint8_t flags, int payload_size);
};

}  // namespace algue::http
