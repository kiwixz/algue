#include "http/server.h"

#include <asio/write.hpp>
#include <kae/exception.h>

#include "http/header_fields.h"
#include "utils/self_path.h"

namespace algue::http {

namespace {

constexpr int read_size = 4096;

}

Server::Server(int port, Dispatch dispatcher) :
    dispatcher_{std::move(dispatcher)},
    acceptor_{io_, {asio::ip::tcp::v4(), static_cast<uint16_t>(port)}}
{
    queue_accept();
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
            logger_(kae::LogLevel::info, "new connection from {}:{}",
                    socket.remote_endpoint().address().to_string(), socket.remote_endpoint().port());
            std::make_shared<Session>(std::move(socket), this)->start();
        }
        else {
            logger_(kae::LogLevel::warning, "error while accepting connection: {}", ec.message());
        }

        queue_accept();
    });
}

Server::Session::Session(asio::ip::tcp::socket socket, Server* server) :
    server_{server},
    request_parser_{MessageType::request},
    socket_{std::move(socket)}
{}

void Server::Session::start()
{
    buffer_.resize(read_size);
    queue_read();
}

void Server::Session::queue_read()
{
    asio::mutable_buffer buf = asio::buffer(buffer_.data() + buffer_.size() - read_size, read_size);
    socket_.async_read_some(buf, [this, self = shared_from_this()](std::error_code ec, size_t size) {
        if (ec == asio::error::eof) {
            logger_(kae::LogLevel::info, "peer disconnected");
            return;
        }
        else if (ec) {
            logger_(kae::LogLevel::error, "error while reading: {}", ec.message());
            return;
        }

        buffer_.resize(buffer_.size() - read_size + size);

        try {
            size_t keep = request_parser_.input(buffer_);

            if (keep > 0 && keep < buffer_.size()) {
                std::copy(buffer_.end() - keep, buffer_.end(), buffer_.begin());
            }
            buffer_.resize(keep + read_size);
        }
        catch (const std::runtime_error& ex) {
            logger_(kae::LogLevel::error, "bad request: {}", ex.what());
            return;
        }

        if (request_parser_.finished()) {
            Request& req = request_parser_.request();
            logger_(kae::LogLevel::info, "request {} {}", req.method, req.path);
            std::vector<std::byte> res = server_->dispatcher_(std::move(req)).serialize();
            asio::write(socket_, asio::buffer(res));
            request_parser_ = Parser{MessageType::request};
        }

        queue_read();
    });
}

}  // namespace algue::http
