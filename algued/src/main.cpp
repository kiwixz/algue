#include <kae/config.h>
#include <kae/exception.h>
#include <kae/os.h>

#include "algued/riot_api_client.h"
#include "http/server.h"
#include "http/status.h"
#include "json/dump.h"

int main(int argc, char** argv)
{
    using namespace algue;
    using namespace algued;

    std::set_terminate(&kae::terminate);
    kae::set_thread_name("MainThread");

    kae::Logger logger{"Daemon"};

    kae::Config cfg;
    cfg.set("web_port", "4444");
    cfg.set("riot_api_key", "");

    if (cfg.parse_args(argc, argv) || argc != 1) {
        cfg.show_help(argv[0]);
        return 1;
    }

    RiotApiClient client{cfg.get_raw("riot_api_key")};
    printf("%s", json::dump(client.get("/lol/summoner/v4/summoners/by-name/shen"), 2).c_str());

    http::Server server{cfg.get<int>("web_port"), [&](http::Request req) {
                            json::Value summoner = client.get("/lol/summoner/v4/summoners/by-name/shen");
                            bool in_game = true;
                            try {
                                client.get("/lol/spectator/v4/active-games/by-summoner/" + summoner["id"].as<std::string>());
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
                        }};
    server.run();
}
