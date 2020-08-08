#include <asio/ip/tcp.hpp>
#include <asio/ssl/stream.hpp>
#include <asio/write.hpp>
#include <kae/config.h>
#include <kae/exception.h>
#include <kae/os.h>

#include "http/connection.h"
#include "http/method.h"
#include "utils/lockfile.h"

namespace {

void async_read(asio::ssl::stream<asio::ip::tcp::socket>& socket, algue::http::Connection& connection)
{
    auto buf = std::make_unique<algue::utils::Bytes>();
    buf->resize(2000);
    asio::mutable_buffers_1 asio_buf = asio::buffer(*buf);
    socket.async_read_some(asio_buf, [&socket, &connection, buf = std::move(buf)](asio::error_code ec, size_t size) {
        if (ec) {
            fmt::print("socket error: {}\n", ec.message());
            exit(1);
        }

        connection.parse({buf->data(), size});
        async_read(socket, connection);
    });
}

}  // namespace


int main(int argc, char** argv)
{
    using namespace algue;

    std::set_terminate(&kae::terminate);
    kae::set_thread_name("MainThread");

    kae::Logger logger{"Explorer"};

    kae::Config cfg;
    cfg.set("data", "");
    cfg.set("method", http::method::get);

    if (cfg.parse_args(argc, argv) || argc != 2) {
        cfg.show_help(argv[0], "path");
        return 1;
    }
    std::string path = argv[1];

    logger(kae::LogLevel::info, "reading lockfile");
    utils::Lockfile lockfile = utils::read_lockfile();

    logger(kae::LogLevel::info, "connecting to 127.0.0.1:{} (token {})", lockfile.port, lockfile.token);
    asio::io_context io_context;
    asio::ssl::context ssl_context{asio::ssl::context::tlsv12_client};
    ssl_context.set_verify_mode(asio::ssl::verify_peer | asio::ssl::verify_fail_if_no_peer_cert);
    ssl_context.load_verify_file("riotgames.pem");

    asio::ssl::stream<asio::ip::tcp::socket> s{io_context, ssl_context};
    std::array<unsigned char, 3> h2 = {{2, 'h', '2'}};
    if (SSL_set_alpn_protos(s.native_handle(), h2.data(), h2.size()))
        throw MAKE_EXCEPTION("cannot set ALPN");

    s.lowest_layer().connect({asio::ip::make_address_v4("127.0.0.1"), static_cast<uint16_t>(lockfile.port)});

    logger(kae::LogLevel::info, "TLS handshake");
    s.handshake(asio::ssl::stream_base::client);

    http::Connection conn;
    asio::write(s, asio::buffer(conn.preface()));

    http::Request req;
    cfg.get_to("method", req.method);
    req.path = path;
    req.headers = lockfile_to_headers(lockfile);
    asio::write(s, asio::buffer(conn.request(std::move(req), [&](http::Response response) {
                    io_context.stop();
                    for (const http::Header& hdr : response.request.headers) {
                        fmt::print(" > {}: {}\n", hdr.name(), hdr.value());
                    }
                    fmt::print("\n");
                    for (const http::Header& hdr : response.headers) {
                        fmt::print(" < {}: {}\n", hdr.name(), hdr.value());
                    }
                    std::string_view data{reinterpret_cast<const char*>(response.data.data()), response.data.size()};
                    fmt::print("\n{}\n", data);
                })));

    async_read(s, conn);
    io_context.run();
}