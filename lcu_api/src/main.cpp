#include <asio/ip/tcp.hpp>
#include <asio/ssl/stream.hpp>
#include <asio/write.hpp>

#include <kae/exception.h>
#include <kae/make_array.h>
#include <kae/os.h>

#include "http/connection.h"
#include "utils/base64.h"
#include "utils/lockfile.h"

namespace {

void async_read(asio::ssl::stream<asio::ip::tcp::socket>& s)
{
    auto buf = std::make_unique<algue::utils::Bytes>();
    buf->resize(2000);
    asio::mutable_buffers_1 asio_buf = asio::buffer(*buf);
    s.async_read_some(asio_buf, [&s, buf = std::move(buf)](asio::error_code ec, size_t size) {
        if (ec == asio::error::eof) {
            exit(0);
        }
        else if (ec) {
            fmt::print("error: {}\n", ec.message());
            exit(1);
        }

        buf->resize(size);
        kae::Logger{"response"}.hexdump(kae::LogLevel::debug, *buf, "response");
        async_read(s);
    });
}

}  // namespace


int main(int /*argc*/, char** /*argv*/)
{
    using namespace algue;

    std::set_terminate(&kae::terminate);
    kae::set_thread_name("main_thread");

    kae::Logger logger{"lcu_api"};

    logger(kae::LogLevel::info, "reading lockfile");
    utils::Lockfile lockfile = utils::read_lockfile();

    logger(kae::LogLevel::info, "connecting to 127.0.0.1:{} (token {})", lockfile.port, lockfile.token);
    asio::io_context io_context;
    asio::ssl::context ssl_context{asio::ssl::context::tlsv12_client};
    ssl_context.set_verify_mode(asio::ssl::verify_peer | asio::ssl::verify_fail_if_no_peer_cert);
    ssl_context.load_verify_file("riotgames.pem");

    asio::ssl::stream<asio::ip::tcp::socket> s{io_context, ssl_context};
    // if (!SSL_set_tlsext_host_name(s.native_handle(), const_cast<char*>("example.org")))
    //     throw std::runtime_error{"no sni"};
    std::array<unsigned char, 3> h2 = {{2, 'h', '2'}};
    if (SSL_set_alpn_protos(s.native_handle(), h2.data(), h2.size()))
        throw std::runtime_error{"cannot set alpn"};

    s.lowest_layer().connect({asio::ip::make_address_v4("127.0.0.1"), static_cast<uint16_t>(lockfile.port)});

    logger(kae::LogLevel::info, "tls handshake");
    s.handshake(asio::ssl::stream_base::client);

    http::Connection conn;
    asio::write(s, asio::buffer(conn.preface()));

    http::Request req;
    req.method = http::Method::get;
    req.path = "/lol-summoner/v1/current-summoner";
    req.headers.push_back({"host", fmt::format("127.0.0.1:{}", lockfile.port)});
    req.headers.push_back({"authorization",
                           fmt::format("Basic {}",
                                       utils::base64(fmt::format("riot:{}", lockfile.token)))});

    asio::write(s, asio::buffer(conn.request(std::move(req))));
    async_read(s);

    io_context.run();
}
