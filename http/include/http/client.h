#pragma once

#include <string_view>

#include <asio/io_context.hpp>
#include <asio/ip/address.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/ssl/context.hpp>
#include <kae/logger.h>

#include "http/request.h"
#include "http/response.h"

namespace algue::http {

struct Client {
    Client(std::string host);

    Response request(Request& request);

private:
    kae::Logger logger_{"HttpClient"};
    std::string host_;
    asio::io_context io_;
    asio::ssl::context ssl_ctx_;
    asio::ip::tcp::resolver::results_type endpoints_;

    void reconnect_socket();
};

}  // namespace algue::http
