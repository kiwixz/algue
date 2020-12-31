#pragma once

#include <string>
#include <string_view>

#include "http/client.h"
#include "http/status.h"
#include "json/value.h"

namespace algue::algued {

struct RiotResponse {
    int status_code;
    json::Value data;
};


struct RiotClient {
    RiotClient(std::string api_key);

    RiotResponse get(std::string_view path);
    json::Value get_data(std::string_view path);

private:
    std::string api_key_;
    http::Client http_;
};

}  // namespace algue::algued
