#pragma once

#include <string_view>

#include "json/value.h"

namespace algue::algued {

struct RiotApiClient {
    json::Value get(std::string_view url);
};

}  // namespace algue::algued
