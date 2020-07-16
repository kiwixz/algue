#include "http/header.h"

#include <array>
#include <cassert>

#include "utils/bytes.h"
#include "utils/endian.h"

namespace algue::http {
namespace {

struct StaticHeader {
    std::string_view name;
    std::string_view value;
};


auto static_table = std::initializer_list<StaticHeader>{
        {":authority", ""},
        {":method", "GET"},
        {":method", "POST"},
        {":path", "/"},
        {":path", "/index.html"},
        {":scheme", "http"},
        {":scheme", "https"},
        {":status", "200"},
        {":status", "204"},
        {":status", "206"},
        {":status", "304"},
        {":status", "400"},
        {":status", "404"},
        {":status", "500"},
        {"accept-charset", ""},
        {"accept-encoding", "gzip, deflate"},
        {"accept-language", ""},
        {"accept-ranges", ""},
        {"accept", ""},
        {"access-control-allow-origin", ""},
        {"age", ""},
        {"allow", ""},
        {"authorization", ""},
        {"cache-control", ""},
        {"content-disposition", ""},
        {"content-encoding", ""},
        {"content-language", ""},
        {"content-length", ""},
        {"content-location", ""},
        {"content-range", ""},
        {"content-type", ""},
        {"cookie", ""},
        {"date", ""},
        {"etag", ""},
        {"expect", ""},
        {"expires", ""},
        {"from", ""},
        {"host", ""},
        {"if-match", ""},
        {"if-modified-since", ""},
        {"if-none-match", ""},
        {"if-range", ""},
        {"if-unmodified-since", ""},
        {"last-modified", ""},
        {"link", ""},
        {"location", ""},
        {"max-forwards", ""},
        {"proxy-authenticate", ""},
        {"proxy-authorization", ""},
        {"range", ""},
        {"referer", ""},
        {"refresh", ""},
        {"retry-after", ""},
        {"server", ""},
        {"set-cookie", ""},
        {"strict-transport-security", ""},
        {"transfer-encoding", ""},
        {"user-agent", ""},
        {"vary", ""},
        {"via", ""},
        {"www-authenticate", ""},
};

}  // namespace


size_t Header::encoded_size() const
{
    return 1 + 1 + name.size() + 1 + value.size();
}

void Header::encode(kae::Span<std::byte> dest) const
{
    assert(!std::any_of(name.begin(), name.end(), [&](char c) { return c >= 'A' && c <= 'Z'; }));

    // compression is not implemented yet
    // neither is integer encoding
    assert(name.size() < 128);
    assert(value.size() < 128);

    utils::BytesWriter writer{dest};
    writer.put(uint8_t{0});
    writer.put(utils::host_to_big(static_cast<uint8_t>(name.size())));
    writer.put(name);
    writer.put(utils::host_to_big(static_cast<uint8_t>(value.size())));
    writer.put(value);

    assert(writer.finished());
}

void Header::decode(kae::Span<const std::byte> src)
{
}


}  // namespace algue::http
