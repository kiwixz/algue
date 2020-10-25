#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <kae/function.h>
#include <kae/logger.h>

#include "http/request.h"
#include "http/response.h"

namespace algue::http {

struct Server {
    using Dispatch = kae::UniqueFunction<Response(Request request)>;

    Server(int port, Dispatch dispatcher);

    void run();
    void stop();

private:
    struct Session : std::enable_shared_from_this<Session> {
        Session(asio::ip::tcp::socket socket, Server* server);

        void start();

    private:
        kae::Logger logger_{"HttpServerSession"};
        Server* server_;
        std::vector<std::byte> buffer_;
        RequestParser request_parser_;
        asio::ip::tcp::socket socket_;

        void queue_read();
    };

    kae::Logger logger_{"HttpServer"};
    Dispatch dispatcher_;
    asio::io_context io_;
    asio::ip::tcp::acceptor acceptor_;

    void queue_accept();
};

}  // namespace algue::http
