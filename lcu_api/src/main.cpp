#include <asio/ip/tcp.hpp>
#include <asio/read.hpp>
#include <asio/ssl/stream.hpp>
#include <asio/write.hpp>

#include <kae/exception.h>
#include <kae/os.h>

#include "utils/base64.h"
#include "utils/lockfile.h"

namespace algue::lcu_api {
namespace {

void async_read(asio::ssl::stream<asio::ip::tcp::socket>& s)
{
    auto buf = std::make_unique<std::vector<char>>();
    buf->resize(2000);
    asio::mutable_buffers_1 asio_buf = asio::buffer(*buf);
    s.async_read_some(asio_buf, [&s, buf = std::move(buf)](asio::error_code ec, size_t /*size*/) {
        if (ec == asio::error::eof) {
            exit(0);
        }
        else if (ec) {
            fmt::print("error: {}\n", ec.message());
            exit(1);
        }
        fmt::print("{}", buf->data());
        async_read(s);
    });
}

}  // namespace
}  // namespace algue::lcu_api


int main(int /*argc*/, char** /*argv*/)
{
    using namespace algue;
    using namespace lcu_api;

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

    s.lowest_layer().connect({asio::ip::make_address_v4("127.0.0.1"), static_cast<uint16_t>(lockfile.port)});

    logger(kae::LogLevel::info, "tls handshake");
    s.handshake(asio::ssl::stream_base::client);

    std::string req = std::string{"GET /lol-summoner/v1/current-summoner HTTP/1.1\r\n"}
                      + "Host: 127.0.0.1:" + fmt::to_string(lockfile.port) + "\r\n"
                      + "Authorization: Basic " + utils::base64("riot:" + lockfile.token) + "\r\n"
                      + "\r\n";

    asio::write(s, asio::buffer(req));
    async_read(s);
    io_context.run();
}
