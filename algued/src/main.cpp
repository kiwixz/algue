#include <kae/config.h>
#include <kae/exception.h>
#include <kae/os.h>

#include "algued/riot_api_client.h"
#include "http/server.h"
#include "json/dump.h"

int main(int argc, char** argv)
{
    using namespace algue;
    using namespace algued;

    std::set_terminate(&kae::terminate);
    kae::set_thread_name("MainThread");

    kae::Logger logger{"Daemon"};

    kae::Config cfg;
    cfg.set("riot_api_key", "");

    if (cfg.parse_args(argc, argv) || argc != 1) {
        cfg.show_help(argv[0]);
        return 1;
    }

    //RiotApiClient client{cfg.get_raw("riot_api_key")};
    //printf("%s", json::dump(client.get("/lol/summoner/v4/summoners/by-name/shen"), 2).c_str());

    http::Server server{4444, [](http::Request req) {
                            printf("%s %s\n", req.method.c_str(), req.path.c_str());
                            http::Response res;
                            res.status_code = 200;
                            res.status_message = "yo";
                            return res;
                        }};
    server.run();
}
