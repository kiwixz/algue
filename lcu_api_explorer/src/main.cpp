#include <asio/ip/tcp.hpp>
#include <asio/ssl/stream.hpp>
#include <asio/write.hpp>
#include <kae/config.h>
#include <kae/exception.h>
#include <kae/os.h>
#include <kae/span.h>

#include "http/methods.h"
#include "http/request.h"
#include "http/response.h"
#include "json/value.h"
#include "utils/lockfile.h"

int main(int argc, char** argv)
{
    using namespace algue;

    std::set_terminate(&kae::terminate);
    kae::set_thread_name("MainThread");

    kae::Logger logger{"Explorer"};

    kae::Config cfg;
    cfg.set("data", "");
    cfg.set("method", http::methods::get);

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

    s.lowest_layer().connect({asio::ip::make_address_v4("127.0.0.1"), static_cast<uint16_t>(lockfile.port)});

    logger(kae::LogLevel::info, "TLS handshake");
    s.handshake(asio::ssl::stream_base::client);

    http::Request req;
    cfg.get_to("method", req.method);
    req.path = path;
    req.headers = lockfile_to_headers(lockfile);

    asio::write(s, asio::buffer(req.serialize()));

    http::Response res;
    res.request = std::move(req);
    res.deserialize([&](kae::Span<std::byte> buffer) {
        return s.read_some(asio::buffer(buffer.data(), buffer.size()));
    });

    fmt::print(" < {} {}\n", res.request.method, res.request.path);
    for (const http::Header& hdr : res.request.headers) {
        fmt::print(" < {}: {}\n", hdr.name, hdr.value);
    }
    fmt::print("\n > {} {}\n", res.status_code, res.status_message);
    for (const http::Header& hdr : res.headers) {
        fmt::print(" > {}: {}\n", hdr.name, hdr.value);
    }
    std::string_view data{reinterpret_cast<const char*>(res.body.data()), res.body.size()};
    fmt::print("\n{}\n", data);

    json::Value v = json::Object{};
    v.as_object()["key"] = true;

    static_assert(std::is_move_constructible_v<json::Value>);
    static_assert(std::is_nothrow_move_constructible_v<json::Value>);
    static_assert(std::is_move_assignable_v<json::Value>);
    static_assert(std::is_nothrow_move_assignable_v<json::Value>);
}
