#pragma once

#include <kae/logger.h>
#include <kae/span.h>

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

    void append_header(utils::Bytes& data, std::string_view name, std::string_view value);
};

}  // namespace algue::http
