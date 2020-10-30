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

        if (request.path == "/live_game") {
            std::string_view input{reinterpret_cast<const char*>(request.body.data()), request.body.size()};
            std::string output = json::dump(live_game(json::parse(input)));
            std::transform(output.begin(), output.end(), std::back_inserter(res.body), [](char c) { return static_cast<std::byte>(c); });
            return res;
        }
    }
    catch (const std::exception& ex) {
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
    json::Value riot_summoner = riot_client_.get(fmt::format("/lol/summoner/v4/summoners/by-name/{}", input["summoner"].as<std::string>()));
    bool in_game = true;
    try {
        riot_client_.get("/lol/spectator/v4/active-games/by-summoner/" + riot_summoner["id"].as<std::string>());
    }
    catch (const std::runtime_error& /*ex*/) {
        in_game = false;
    }

    json::Object o;
    o["in_game"] = in_game;
    return o;
}

json::Value Dispatcher::live_game(const json::Value& input)
{
    auto team = [](int team_id) -> std::string {
        if (team_id == 100)
            return "blue";
        else if (team_id == 200)
            return "red";
        else
            return fmt::format("{}", team_id);
    };

    json::Value riot_summoner = riot_client_.get(fmt::format("/lol/summoner/v4/summoners/by-name/{}", input["summoner"].as<std::string>()));
    json::Value riot_game = riot_client_.get("/lol/spectator/v4/active-games/by-summoner/" + riot_summoner["id"].as<std::string>());

    json::Object o;
    o["mode"] = riot_game["gameMode"];
    o["type"] = riot_game["gameType"];
    o["map"] = riot_game["mapId"];
    o["start_time"] = riot_game["gameStartTime"];
    o["queue_type"] = riot_game["gameQueueConfigId"];

    json::Array& riot_bans = riot_game["bannedChampions"].as<json::Array>();
    json::Array& bans = o.try_emplace("bans", json::Array{}).first->second.as<json::Array>();
    std::transform(riot_bans.begin(), riot_bans.end(), std::back_inserter(bans), [&](const json::Value& b) {
        json::Object r;
        r["team"] = team(b["teamId"].get<int>());
        r["champion"] = b["championId"];
        return r;
    });

    json::Array& riot_players = riot_game["participants"].as<json::Array>();
    json::Object& players = o.try_emplace("players", json::Object{}).first->second.as<json::Object>();
    for (json::Value& p : riot_players) {
        json::Object& r = players.try_emplace(team(p["teamId"].get<int>()), json::Object{}).first->second.as<json::Object>();
        r["is_bot"] = p["bot"];
        r["summoner_name"] = p["summonerName"];
        r["profile_icon"] = p["profileIconId"];
        r["champion"] = p["championId"];

        json::Array& spells = o.try_emplace("spells", json::Array{}).first->second.as<json::Array>();
        spells[0] = p["spell1Id"];
        spells[1] = p["spell2Id"];

        json::Object& runes = o.try_emplace("runes", json::Object{}).first->second.as<json::Object>();
        runes["primary"] = p["perkStyle"];
        runes["secondary"] = p["perkSubStyle"];
        runes["ids"] = p["perkIds"];
    }

    return o;
}

}  // namespace algue::algued
