#pragma once

#include <string>
#include <string_view>

#include "http/client.h"
#include "json/value.h"

namespace algue::algued {

struct RiotClient {
    RiotClient(std::string api_key);

    json::Value get(std::string_view path);

private:
    std::string api_key_;
    http::Client http_;
};

}  // namespace algue::algued
