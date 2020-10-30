#pragma once

#include <kae/logger.h>

#include "algued/riot_client.h"
#include "http/request.h"
#include "http/response.h"

namespace algue::algued {

struct Dispatcher {
    Dispatcher(std::string riot_api_key);

    http::Response operator()(const http::Request& request);

private:
    kae::Logger logger_{"Dispatcher"};
    RiotClient riot_client_;

    json::Value is_live(const json::Value& input);
    json::Value live_game(const json::Value& input);
};

}  // namespace algue::algued
