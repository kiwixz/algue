#include <charconv>
#include <fstream>

#include <asio/ip/tcp.hpp>
#include <asio/read.hpp>
#include <asio/ssl/stream.hpp>
#include <asio/write.hpp>

#include <kae/exception.h>
#include <kae/os.h>

namespace {

std::string base64(std::string_view input)
{
    constexpr std::string_view kEncodeLookup = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    constexpr char kPadCharacter = '=';

    std::string encoded;
    encoded.reserve(((input.size() / 3) + (input.size() % 3 > 0)) * 4);

    std::uint32_t temp{};
    const char* it = input.data();

    for (std::size_t i = 0; i < input.size() / 3; ++i) {
        temp = (*it++) << 16;
        temp += (*it++) << 8;
        temp += (*it++);
        encoded.append(1, kEncodeLookup[(temp & 0x00FC0000) >> 18]);
        encoded.append(1, kEncodeLookup[(temp & 0x0003F000) >> 12]);
        encoded.append(1, kEncodeLookup[(temp & 0x00000FC0) >> 6]);
        encoded.append(1, kEncodeLookup[(temp & 0x0000003F)]);
    }

    switch (input.size() % 3) {
    case 1:
        temp = (*it++) << 16;
        encoded.append(1, kEncodeLookup[(temp & 0x00FC0000) >> 18]);
        encoded.append(1, kEncodeLookup[(temp & 0x0003F000) >> 12]);
        encoded.append(2, kPadCharacter);
        break;
    case 2:
        temp = (*it++) << 16;
        temp += (*it++) << 8;
        encoded.append(1, kEncodeLookup[(temp & 0x00FC0000) >> 18]);
        encoded.append(1, kEncodeLookup[(temp & 0x0003F000) >> 12]);
        encoded.append(1, kEncodeLookup[(temp & 0x00000FC0) >> 6]);
        encoded.append(1, kPadCharacter);
        break;
    }

    return encoded;
}

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

    // read lockfile
    uint16_t port = 0;
    std::string token;

    {
        std::ifstream ifs{"C:/Riot Games/League of Legends/lockfile", std::ios::ate};
        if (!ifs)
            throw std::runtime_error{"no lockfile, is league launched?"};

        std::string file;
        file.resize(ifs.tellg());
        ifs.seekg(0, std::ios::beg);
        ifs.read(file.data(), file.size());

        fmt::print("{}\n", file);

        auto next = [&](size_t start) {
            size_t r = file.find(':', start);
            if (r == std::string::npos)
                throw std::runtime_error{"corrupt lockfile"};
            return r + 1;
        };

        size_t idx_port = next(next(0));
        size_t idx_token = next(idx_port);
        size_t end_token = next(idx_token);

        if (std::from_chars(file.data() + idx_port, file.data() + idx_token - 1, port).ptr != file.data() + idx_token - 1)
            throw std::runtime_error{"could not parse lockfile port"};

        token = file.substr(idx_token, end_token - idx_token - 1);
    }

    fmt::print("port = {}, token = {}\n", port, token);

    asio::io_context io_context;
    asio::ssl::context ssl_context{asio::ssl::context::tlsv12_client};
    ssl_context.set_verify_mode(asio::ssl::verify_peer | asio::ssl::verify_fail_if_no_peer_cert);
    ssl_context.load_verify_file("riotgames.pem");

    asio::ssl::stream<asio::ip::tcp::socket> s{io_context, ssl_context};
    // if (!SSL_set_tlsext_host_name(s.native_handle(), const_cast<char*>("example.org")))
    //     throw std::runtime_error{"no sni"};

    s.lowest_layer().connect({asio::ip::make_address_v4("127.0.0.1"), port});
    s.handshake(asio::ssl::stream_base::client);

    std::string req = std::string{"GET /lol-summoner/v1/current-summoner HTTP/1.1\r\n"}
                      + "Host: 127.0.0.1:" + fmt::to_string(port) + "\r\n"
                      + "Authorization: Basic " + base64("riot:" + token) + "\r\n"
                      + "\r\n";

    fmt::print(req);

    asio::write(s, asio::buffer(req));
    async_read(s);
    io_context.run();
}
