#include "http/client.h"

#include <asio/connect.hpp>
#include <asio/ssl/host_name_verification.hpp>
#include <asio/ssl/stream.hpp>
#include <asio/write.hpp>

#include "http/header_fields.h"
#include "http/parser.h"
#include "utils/self_path.h"

namespace algue::http {

Client::Client(std::string host) :
    host_{std::move(host)}, ssl_ctx_{asio::ssl::context::tls}
{
    ssl_ctx_.set_options(asio::ssl::context::no_sslv2
                         | asio::ssl::context::no_sslv3
                         | asio::ssl::context::no_tlsv1
                         | asio::ssl::context::no_tlsv1_1);
    ssl_ctx_.load_verify_file((utils::get_self_path().parent_path().parent_path() / "data/cacert.pem").string());
    ssl_ctx_.set_verify_mode(asio::ssl::verify_peer | asio::ssl::verify_fail_if_no_peer_cert);
    ssl_ctx_.set_verify_callback(asio::ssl::host_name_verification{host_});

    asio::ip::tcp::resolver resolver{io_};
    endpoints_ = resolver.resolve(host_, "443");
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

    http::Parser parser{MessageType::response};
    int read_size = 4096;
    std::vector<std::byte> buf;
    buf.resize(read_size);
    while (!parser.finished()) {
        size_t size = s.read_some(asio::buffer(buf.data() + buf.size() - read_size, read_size));
        buf.resize(buf.size() - read_size + size);
        size_t keep = buf.size() - parser.input(buf);
        if (keep > 0 && keep < buf.size()) {
            std::copy(buf.end() - keep, buf.end(), buf.begin());
        }
        buf.resize(keep + read_size);
    }

    try {
        s.shutdown();
    }
    catch (const std::system_error& /*ex*/) {
    }
    return std::move(parser.response());
}

}  // namespace algue::http
