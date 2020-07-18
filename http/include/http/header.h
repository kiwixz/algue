#pragma once

#include <array>
#include <deque>
#include <string>
#include <string_view>
#include <variant>

#include <kae/span.h>

#include "utils/bytes.h"
#include "utils/lower_bound.h"

namespace algue::http {

struct StaticHeader {
    std::string_view name;
    std::string_view value;
};

inline constexpr std::array<StaticHeader, 61> static_header_table = {{
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
}};


struct Header {
    using StaticTableIndex = size_t;

    enum class ShouldIndex {
        yes,
        no,
        never,
    };

    static constexpr StaticTableIndex name_index(std::string_view name)
    {
        auto it = utils::lower_bound_if(static_header_table, [&](const StaticHeader& hdr) { return hdr.name < name; });
        if (it == static_header_table.end() || it->name != name)
            throw std::runtime_error{"header name not found in static table"};
        return it - static_header_table.begin();
    }

    Header() = default;
    Header(StaticTableIndex name_index, std::string value, ShouldIndex should_index = ShouldIndex::yes);
    Header(std::string name, std::string value, ShouldIndex should_index = ShouldIndex::yes);

    std::string_view name() const;
    std::string_view value() const;

    size_t encoded_size() const;
    void encode(kae::Span<std::byte> dest) const;
    kae::Span<const std::byte> decode(kae::Span<const std::byte> src);

private:
    std::variant<StaticTableIndex, std::string> name_;
    std::string value_;
    ShouldIndex should_index_;

    kae::Span<const std::byte> decode_name_value(kae::Span<const std::byte> src, uint64_t name_index);
    uint64_t consume_int(kae::Span<const std::byte>& src, int prefix);
    std::string consume_str(kae::Span<const std::byte>& src);
    void put_int(utils::BytesWriter& writer, uint8_t first_byte, int prefix, uint64_t n) const;
    void put_str(utils::BytesWriter& writer, std::string_view str) const;
};


struct HeaderTable {
private:
    std::deque<Header> list_;
};

}  // namespace algue::http
