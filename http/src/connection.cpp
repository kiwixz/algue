#include "http/connection.h"

template <class... TBases>
struct Overloaded : TBases... {
    using TBases::operator()...;
};
template <class... TBases>
Overloaded(TBases...) -> Overloaded<TBases...>;

namespace algue::http {

std::vector<std::byte> Connection::make_request(Request request)
{
    std::vector<std::byte> data;

    auto append = Overloaded{[&](kae::Span<const std::byte> more) {
                                 data.insert(data.end(), more.begin(), more.end());
                             },
                             [&](std::string_view more) {
                                 const std::byte* ptr = reinterpret_cast<const std::byte*>(more.data());
                                 data.insert(data.end(), ptr, ptr + more.size());
                             },
                             [&](const std::string& more) {
                                 const std::byte* ptr = reinterpret_cast<const std::byte*>(more.data());
                                 data.insert(data.end(), ptr, ptr + more.size());
                             }};

    std::string_view space = " ";
    std::string_view version = "HTTP/1.1";
    std::string_view eol = "\r\n";
    std::string_view header_def = ": ";

    append(to_string(request.method));
    append(space);
    append(request.path);
    append(space);
    append(version);
    append(eol);

    for (const Header& header : request.headers) {
        append(header.name);
        append(header_def);
        append(header.value);
        append(eol);
    }

    append(eol);

    logger(kae::LogLevel::debug, "making http request:\n{}",
           std::string_view{reinterpret_cast<const char*>(data.data()), data.size()});

    return data;
}

Response Connection::parse(kae::Span<const std::byte> data)
{
    return {};
}

}  // namespace algue::http
