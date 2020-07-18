#include "http/header.h"

#include <cassert>
#include <string>
#include <string_view>

#include <kae/bit_cast.h>
#include <kae/logger.h>

#include "http/huffman.h"
#include "utils/bytes.h"
#include "utils/endian.h"

namespace algue::http {

Header::Header(StaticTableIndex name_index, std::string value, ShouldIndex should_index) :
    name_{name_index}, value_{value}, should_index_{should_index}
{}

Header::Header(std::string name, std::string value, ShouldIndex should_index) :
    name_{name}, value_{value}, should_index_{should_index}
{}


std::string_view Header::name() const
{
    if (auto* idx = std::get_if<StaticTableIndex>(&name_))
        return static_header_table[*idx].name;
    else
        return std::get<std::string>(name_);
}

std::string_view Header::value() const
{
    return value_;
}

size_t Header::encoded_size() const
{
    size_t name_size = std::min(name().size(), huffman_size(name()));
    size_t value_size = std::min(value().size(), huffman_size(value()));
    assert(name_size < 128);  // size compute of int over multiple bytes not implemented
    assert(value_size < 128);
    return 1 + 1 + name_size + 1 + value_size;
}

void Header::encode(kae::Span<std::byte> dest) const
{
    assert(!std::any_of(name().begin(), name().end(), [&](char c) { return c >= 'A' && c <= 'Z'; }));

    utils::BytesWriter writer{dest};
    writer.put(uint8_t{0});
    put_str(writer, name());
    put_str(writer, value());

    assert(writer.finished());
}

kae::Span<const std::byte> Header::decode(kae::Span<const std::byte> src)
{
    auto first = kae::bit_cast<uint8_t>(src[0]);

    if (first & 0x80)  // fully indexed
    {
        should_index_ = ShouldIndex::yes;
        uint64_t idx = consume_int(src, 7) - 1;
        assert(idx < static_header_table.size());  // dynamic table not yet implemented
        name_ = idx;
        value_ = static_header_table[idx].value;
    }
    else if ((first & 0xc0) == 0x40)  // literal with indexing
    {
        should_index_ = ShouldIndex::yes;
        uint64_t name_index = consume_int(src, 6);
        src = decode_name_value(src, name_index);
    }
    else if ((first & 0xf0) == 0)  // literal without indexing
    {
        should_index_ = ShouldIndex::no;
        uint64_t name_index = consume_int(src, 4);
        src = decode_name_value(src, name_index);
    }
    else if ((first & 0xf0) == 0x10)  // literal never indexed
    {
        should_index_ = ShouldIndex::never;
        uint64_t name_index = consume_int(src, 4);
        src = decode_name_value(src, name_index);
    }
    else if ((first & 0x20) == 0x20)  // table size update
    {
        //uint64_t max_size = consume_int(src, 5);
        assert(false);
    }
    else
        throw std::runtime_error{"bad header block"};

    return src;
}

kae::Span<const std::byte> Header::decode_name_value(kae::Span<const std::byte> src, uint64_t name_index)
{
    if (name_index == 0) {  // name not indexed
        name_ = consume_str(src);
    }
    else {
        --name_index;
        assert(name_index < static_header_table.size());  // dynamic table not yet implemented
        name_ = name_index;
    }
    value_ = consume_str(src);
    return src;
}

uint64_t Header::consume_int(kae::Span<const std::byte>& src, int prefix)
{
    assert(src.size() >= 1);
    uint8_t max_prefix = static_cast<uint8_t>(0xff >> (8 - prefix));
    uint64_t value = kae::bit_cast<uint8_t>(src[0]) & max_prefix;
    if (value != max_prefix) {
        src = src.subspan(1, src.size() - 1);
        return value;
    }

    size_t idx = 1;
    while (true) {
        assert(src.size() > idx);
        auto next_byte = kae::bit_cast<uint8_t>(src[idx]);
        value = value + (next_byte & 0x7f) * (1ull << ((idx - 1) * 7));
        ++idx;
        if ((next_byte & 0x80) != 0x80)
            break;
    }

    src = src.subspan(idx, src.size() - idx);
    return value;
}

std::string Header::consume_str(kae::Span<const std::byte>& src)
{
    bool huffman = kae::bit_cast<uint8_t>(src[0]) & 0x80;
    uint64_t length = consume_int(src, 7);

    assert(src.size() >= length);
    std::string r;
    if (huffman) {
        r = huffman_decode(src.subspan(0, length));
    }
    else {
        r.resize(length);
        std::transform(src.begin(), src.begin() + length, r.begin(), [&](std::byte b) { return static_cast<char>(b); });
    }

    src = src.subspan(length, src.size() - length);
    return r;
}

void Header::put_int(utils::BytesWriter& writer, uint8_t first_byte, int prefix, uint64_t n) const
{
    uint8_t max_first = 0xff >> (8 - prefix);
    if (n < max_first) {
        writer.put(static_cast<uint8_t>(n | first_byte));
        return;
    }

    writer.put(static_cast<uint8_t>(max_first | first_byte));

    n -= max_first;
    while (n >= 128) {
        writer.put(static_cast<uint8_t>(n | 0x80));
        n >>= 7;
    }
    writer.put(static_cast<uint8_t>(n));
}

void Header::put_str(utils::BytesWriter& writer, std::string_view str) const
{
    size_t encoded_size = huffman_size(str);
    if (huffman_size(str) < str.size()) {
        put_int(writer, 0x80, 7, encoded_size);
        huffman_encode(str, writer.put_zero(encoded_size));
    }
    else {
        put_int(writer, 0, 7, str.size());
        writer.put(str);
    }
}

}  // namespace algue::http
