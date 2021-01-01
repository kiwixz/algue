#pragma once

#include <memory>
#include <string_view>

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <kae/function.h>
#include <kae/logger.h>

#include "http/parser.h"
#include "http/request.h"
#include "http/response.h"
#include "utils/bytes.h"

namespace algue::http {

struct Server {
    using Dispatch = kae::UniqueFunction<Response(Request request)>;

    Server(int port, Dispatch dispatcher);

    asio::ip::tcp::endpoint endpoint() const;

    void run();
    void stop();

private:
    struct Session : std::enable_shared_from_this<Session> {
        Session(asio::ip::tcp::socket socket, Server* server);

        void start();

    private:
        kae::Logger logger_{"HttpServerSession"};
        Server* server_;
        utils::Bytes buffer_;
        Parser request_parser_;
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
