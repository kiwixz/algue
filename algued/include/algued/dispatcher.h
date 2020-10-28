#pragma once

#include "algued/riot_client.h"
#include "http/request.h"
#include "http/response.h"

namespace algue::algued {

struct Dispatcher {
    Dispatcher(std::string riot_api_key);

    http::Response operator()(const http::Request& request);

private:
    RiotClient riot_client_;

    json::Value is_live(const json::Value& input);
};

}  // namespace algue::algued
