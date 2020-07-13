#pragma once

#include <vector>

#include <kae/logger.h>
#include <kae/span.h>

#include "http/request.h"
#include "http/response.h"

namespace algue::http {

struct Connection {
    std::vector<std::byte> make_request(Request request);
    Response parse(kae::Span<const std::byte> data);

private:
    kae::Logger logger{"Connection"};
};

}  // namespace algue::http
