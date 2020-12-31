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
        logger_(kae::LogLevel::error, "unsupported method '{}'", request.method);
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

        if (request.path == "/live_game") {
            std::string_view input{reinterpret_cast<const char*>(request.body.data()), request.body.size()};
            std::string output = json::dump(live_game(json::parse(input)));
            std::transform(output.begin(), output.end(), std::back_inserter(res.body), [](char c) { return static_cast<std::byte>(c); });
            return res;
        }
    }
    catch (const std::exception& ex) {
        logger_(kae::LogLevel::error, "caught exception: {}", ex.what());
        http::Response res;
        res.status_code = http::Status::internal_server_error;
        res.status_message = "error";
        res.headers.push_back({http::header_fields::content_type, "application/json"});
        std::string body = json::dump(json::Object{{{"message", ex.what()}}});
        std::transform(body.begin(), body.end(), std::back_inserter(res.body), [](char c) { return static_cast<std::byte>(c); });
        return res;
    }

    http::Response res;
    res.status_code = http::Status::not_found;
    res.status_message = "unknown path";
    return res;
}

json::Value Dispatcher::is_live(const json::Value& input)
{
    RiotResponse riot_summoner = riot_client_.get(fmt::format("/lol/summoner/v4/summoners/by-name/{}", input["summoner"].as<std::string>()));
    if (riot_summoner.status_code != http::Status::ok)
        return json::Object{{{"exists", false}}};
    RiotResponse riot_game = riot_client_.get("/lol/spectator/v4/active-games/by-summoner/" + riot_summoner.data["id"].as<std::string>());
    return json::Object{{{"exists", true}, {"in_game", riot_game.status_code == http::Status::ok}}};
}

json::Value Dispatcher::live_game(const json::Value& input)
{
    json::Value riot_summoner = riot_client_.get_data(fmt::format("/lol/summoner/v4/summoners/by-name/{}", input["summoner"].as<std::string>()));
    json::Value riot_game = riot_client_.get_data("/lol/spectator/v4/active-games/by-summoner/" + riot_summoner["id"].as<std::string>());

    json::Object o;
    o["queue_type"] = riot_game["gameQueueConfigId"];
    o["map"] = riot_game["mapId"];
    o["start_time"] = riot_game["gameStartTime"].get<unsigned long long>() / 1000;

    json::Array& riot_bans = riot_game["bannedChampions"].as<json::Array>();
    json::Array& bans = o.try_emplace("bans", json::Array{}).first->second.as<json::Array>();
    std::transform(riot_bans.begin(), riot_bans.end(), std::back_inserter(bans), [&](const json::Value& b) {
        json::Object r;
        r["team"] = b["teamId"].get<int>();
        r["champion"] = b["championId"];
        return r;
    });

    json::Array& riot_players = riot_game["participants"].as<json::Array>();
    json::Object& players = o.try_emplace("players", json::Object{}).first->second.as<json::Object>();
    for (json::Value& p : riot_players) {
        json::Array& team_players = players.try_emplace(fmt::format("{}", p["teamId"].get<int>()),
                                                        json::Array{})
                                            .first->second.as<json::Array>();
        json::Object& r = team_players.emplace_back(json::Object{}).as<json::Object>();
        //r["is_bot"] = p["bot"];
        r["summoner_name"] = p["summonerName"];
        //r["profile_icon"] = p["profileIconId"];
        r["champion"] = p["championId"];

        // json::Array& spells = r.try_emplace("spells", json::Array{}).first->second.as<json::Array>();
        // spells.push_back(p["spell1Id"]);
        // spells.push_back(p["spell2Id"]);

        // json::Value& riot_runes = p["perks"];
        // json::Object& runes = r.try_emplace("runes", json::Object{}).first->second.as<json::Object>();
        // runes["primary"] = riot_runes["perkStyle"];
        // runes["secondary"] = riot_runes["perkSubStyle"];
        // runes["ids"] = riot_runes["perkIds"];
    }

    return o;
}

}  // namespace algue::algued
