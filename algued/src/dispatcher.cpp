#include "algued/dispatcher.h"

#include "http/status.h"
#include "json/dump.h"
#include "json/parse.h"

namespace algue::algued {

Dispatcher::Dispatcher(std::string riot_api_key) :
    riot_client_{std::move(riot_api_key)}
{
}

http::Response Dispatcher::operator()(const http::Request& request)
{
    (void)request;

    json::Value summoner = riot_client_.get("/lol/summoner/v4/summoners/by-name/shen");
    bool in_game = true;
    try {
        riot_client_.get("/lol/spectator/v4/active-games/by-summoner/" + summoner["id"].as<std::string>());
    }
    catch (const std::runtime_error& /*ex*/) {
        in_game = false;
    }

    json::Object o;
    o["in_game"] = in_game;

    http::Response res;
    res.status_code = http::Status::ok;
    res.status_message = "ok";
    std::string j = json::dump(o);
    std::transform(j.begin(), j.end(), std::back_inserter(res.body), [](char c) { return static_cast<std::byte>(c); });
    return res;
}

}  // namespace algue::algued
