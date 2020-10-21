#include "http/client.h"

#include <asio/connect.hpp>
#include <asio/ssl/host_name_verification.hpp>
#include <asio/ssl/stream.hpp>
#include <asio/write.hpp>

#include "http/header_fields.h"

namespace algue::http {

Client::Client(std::string host) :
    host_{std::move(host)}, ssl_ctx_{asio::ssl::context::tls}
{
    ssl_ctx_.set_options(asio::ssl::context::no_sslv2
                         | asio::ssl::context::no_sslv3
                         | asio::ssl::context::no_tlsv1
                         | asio::ssl::context::no_tlsv1_1);
    ssl_ctx_.set_default_verify_paths();
    ssl_ctx_.set_verify_mode(asio::ssl::verify_peer | asio::ssl::verify_fail_if_no_peer_cert);
    ssl_ctx_.set_verify_callback(asio::ssl::host_name_verification{host_});

    asio::ip::tcp::resolver resolver{io_};
    endpoints_ = resolver.resolve(host_, "https");
}

http::Response Client::request(http::Request& request)
{
    if (!request.has_header(header_fields::host)) {
        request.headers.push_back({std::string{header_fields::host}, host_});
    }

    asio::ssl::stream<asio::ip::tcp::socket> s{io_, ssl_ctx_};
    asio::ip::tcp::endpoint peer = asio::connect(s.lowest_layer(), endpoints_);
    logger_(kae::LogLevel::debug, "connected to {}:{}", peer.address().to_string(), peer.port());

    if (!SSL_set_tlsext_host_name(s.native_handle(), const_cast<char*>(host_.c_str()))) {
        logger_(kae::LogLevel::warning, "TLS SNI is not supported");
    }
    s.handshake(asio::ssl::stream_base::client);
    asio::write(s, asio::buffer(request.serialize()));

    http::Response res;
    res.deserialize([&](std::span<std::byte> buffer) {
        return s.read_some(asio::buffer(buffer.data(), buffer.size()));
    });
    s.shutdown();
    return res;
}

}  // namespace algue::http
