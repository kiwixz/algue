#include "algued/dispatcher.h"

#include "http/header_fields.h"
#include "http/methods.h"
#include "http/status.h"
#include "json/dump.h"
#include "json/parse.h"
#include "json/value.h"

namespace algue::algued {

Dispatcher::Dispatcher(std::string riot_api_key) :
    riot_client_{std::move(riot_api_key)}
{
}

http::Response Dispatcher::operator()(const http::Request& request)
{
    if (request.method != http::methods::post) {
        http::Response res;
        res.status_code = http::Status::method_not_allowed;
        res.status_message = "only POST is supported";
        return res;
    }

    try
    {
        http::Response res;
        res.status_code = http::Status::ok;
        res.status_message = "ok";
        res.headers.push_back({http::header_fields::content_type, "application/json"});

        if (request.path == "/is_live") {
            std::string_view input{reinterpret_cast<const char*>(request.body.data()), request.body.size()};
            std::string output = json::dump(is_live(json::parse(input)));
            std::transform(output.begin(), output.end(), std::back_inserter(res.body), [](char c) { return static_cast<std::byte>(c); });
            return res;
        }
    }
    catch (const std::runtime_error& ex) {
        http::Response res;
        res.status_code = http::Status::internal_server_error;
        res.status_message = "error";
        std::string_view reason = ex.what();
        std::transform(reason.begin(), reason.end(), std::back_inserter(res.body), [](char c) { return static_cast<std::byte>(c); });
        return res;
    }

    http::Response res;
    res.status_code = http::Status::not_found;
    res.status_message = "unknown path";
    return res;
}

json::Value Dispatcher::is_live(const json::Value& input)
{
    json::Value summoner = riot_client_.get(fmt::format("/lol/summoner/v4/summoners/by-name/{}", input["summoner"].as<std::string>()));
    bool in_game = true;
    try {
        riot_client_.get("/lol/spectator/v4/active-games/by-summoner/" + summoner["id"].as<std::string>());
    }
    catch (const std::runtime_error& /*ex*/) {
        in_game = false;
    }

    json::Object o;
    o["in_game"] = in_game;
    return o;
}

}  // namespace algue::algued