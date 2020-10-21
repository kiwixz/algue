#include <kae/config.h>
#include <kae/exception.h>
#include <kae/os.h>

#include "http/client.h"
#include "http/methods.h"
#include "http/request.h"
#include "http/response.h"
#include "json/dump.h"

int main(int argc, char** argv)
{
    using namespace algue;

    std::set_terminate(&kae::terminate);
    kae::set_thread_name("MainThread");

    kae::Logger logger{"Daemon"};

    kae::Config cfg;
    cfg.set("riot_api_key", "");

    if (cfg.parse_args(argc, argv) || argc != 1) {
        cfg.show_help(argv[0]);
        return 1;
    }

    http::Request req;
    req.method = http::methods::get;
    req.path = "/lol/summoner/v4/summoners/by-name/shen";
    http::Response res = http::Client{"euw1.api.riotgames.com"}.request(req);

    fmt::print(" < {} {}\n", req.method, req.path);
    for (const http::Header& hdr : req.headers) {
        fmt::print(" < {}: {}\n", hdr.name, hdr.value);
    }
    fmt::print("\n > {} {}\n", res.status_code, res.status_message);
    for (const http::Header& hdr : res.headers) {
        fmt::print(" > {}: {}\n", hdr.name, hdr.value);
    }
    std::string_view data{reinterpret_cast<const char*>(res.body.data()), res.body.size()};
    fmt::print("\n{}", json::format(data, 2));
}
