#include <kae/config.h>
#include <kae/exception.h>
#include <kae/os.h>

#include "algued/dispatcher.h"
#include "http/server.h"

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

    Dispatcher dispatcher{cfg.get_raw("riot_api_key")};
    http::Server server{cfg.get<int>("web_port"), std::bind_front(std::ref(dispatcher))};
    server.run();
}
