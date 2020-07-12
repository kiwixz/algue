#include <asio/ip/tcp.hpp>
#include <asio/read.hpp>
#include <asio/ssl/stream.hpp>
#include <asio/write.hpp>

#include <kae/exception.h>
#include <kae/os.h>

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

int main(int /*argc*/, char** /*argv*/)
{
    std::set_terminate(&kae::terminate);
    kae::set_thread_name("main_thread");

    asio::io_context io_context;
    asio::ssl::context ssl_context{asio::ssl::context::tlsv13_client};
    ssl_context.load_verify_file("../example.pem");

    asio::ssl::stream<asio::ip::tcp::socket> s{io_context, ssl_context};
    s.lowest_layer().connect({asio::ip::make_address_v4("93.184.216.34"), 80});
    s.handshake(asio::ssl::stream_base::client);

    asio::write(s, asio::buffer("GET / HTTP/1.1\nHost: example.org\n\n"));
    async_read(s);
    io_context.run();
}
