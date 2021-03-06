#include "algued/riot_client.h"

#include "algued/riot_client_fake.h"
#include "http/methods.h"
#include "http/request.h"
#include "http/response.h"
#include "http/status.h"
#include "json/parse.h"

namespace algue::algued {

RiotClient::RiotClient(std::string api_key) :
    api_key_{std::move(api_key)}, http_{"euw1.api.riotgames.com"}
{}

RiotResponse RiotClient::get(std::string_view path)
{
#ifdef DEBUG
    std::optional<std::string_view> fake_response = get_riot_client_fake(path);
    if (fake_response)
        return {http::ok, json::parse(*fake_response)};
#endif

    http::Request req;
    req.method = http::methods::get;
    req.path = path;
    req.headers.push_back({"x-riot-token", api_key_});
    http::Response res = http_.request(req);
    return {res.status_code, json::parse(res.body.as_chars())};
}

json::Value RiotClient::get_data(std::string_view path)
{
    RiotResponse res = get(path);
    if (http::status_class(res.status_code) != http::StatusClass::success)
        throw MAKE_EXCEPTION("response has non-success status code {}", res.status_code);
    return res.data;
}

}  // namespace algue::algued
