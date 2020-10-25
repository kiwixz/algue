#include "http/server.h"

#include <asio/write.hpp>
#include <kae/exception.h>

#include "http/header_fields.h"
#include "utils/self_path.h"

namespace algue::http {

Server::Server(int port) :
    acceptor_{io_, {asio::ip::tcp::v4(), static_cast<uint16_t>(port)}}
{
    queue_accept();
}

void Server::add_handler(std::string path, Handler handler)
{
    if (!handlers_.try_emplace(std::move(path), std::move(handler)).second)
        throw MAKE_EXCEPTION("duplicate handler path");
}

void Server::run()
{
    io_.run();
}

void Server::stop()
{
    io_.stop();
}

void Server::queue_accept()
{
    acceptor_.async_accept([this](std::error_code ec, asio::ip::tcp::socket socket) {
        if (!ec) {
            std::make_shared<Session>(std::move(socket))->start();
        }
        else {
            logger_(kae::LogLevel::warning, "error while accepting connection: {}", ec.message());
        }

        queue_accept();
    });
}

Server::Session::Session(asio::ip::tcp::socket socket) :
    socket_{std::move(socket)}
{}

void Server::Session::start()
{
}

}  // namespace algue::http
