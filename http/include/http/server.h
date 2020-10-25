#pragma once

#include <string_view>
#include <unordered_map>
#include <vector>

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <kae/function.h>
#include <kae/logger.h>

#include "http/request.h"
#include "http/response.h"

namespace algue::http {

struct Server {
    using Handler = kae::UniqueFunction<Response(Request request)>;

    Server(int port);

    void add_handler(std::string path, Handler handler);

    void run();
    void stop();

private:
    struct Session {
        Session(asio::ip::tcp::socket socket);

        asio::ip::tcp::socket socket_;

        void start();
    };

    kae::Logger logger_{"HttpServer"};
    std::unordered_map<std::string, Handler> handlers_;
    asio::io_context io_;
    asio::ip::tcp::acceptor acceptor_;

    void queue_accept();
};

}  // namespace algue::http
