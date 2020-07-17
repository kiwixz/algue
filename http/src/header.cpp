#include "http/header.h"

#include <cassert>
#include <string>
#include <string_view>

#include <kae/bit_cast.h>
#include <kae/logger.h>

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
    return 1 + 1 + name().size() + 1 + value().size();
}

void Header::encode(kae::Span<std::byte> dest) const
{
    assert(!std::any_of(name().begin(), name().end(), [&](char c) { return c >= 'A' && c <= 'Z'; }));

    // compression is not implemented yet
    // neither is integer encoding
    assert(name().size() < 128);
    assert(value().size() < 128);

    utils::BytesWriter writer{dest};
    writer.put(uint8_t{0});
    writer.put(utils::host_to_big(static_cast<uint8_t>(name().size())));
    writer.put(name());
    writer.put(utils::host_to_big(static_cast<uint8_t>(value().size())));
    writer.put(value());

    assert(writer.finished());
}

kae::Span<const std::byte> Header::decode(kae::Span<const std::byte> src)
{
    kae::Logger logger{"header_decode"};

    auto first = utils::big_to_host(kae::bit_cast<uint8_t>(src[0]));

    if (first & 0x80)  // fully indexed
    {
        logger(kae::LogLevel::debug, "fully indexed");
        uint64_t idx = consume_int(src, 7);
        logger(kae::LogLevel::debug, "{}", idx);
        return src;
    }

    if ((first & 0xc0) == 0x40)  // literal with indexing
    {
        logger.hexdump(kae::LogLevel::debug, src, "literal with indexing");

        uint64_t idx = consume_int(src, 6);
        if (idx) {  // indexed name
            std::string value = consume_str(src);
            logger(kae::LogLevel::debug, "{} = '{}'", idx, value);
        }
        else {
            std::string name = consume_str(src);
            std::string value = consume_str(src);
            logger(kae::LogLevel::debug, "'{}' = '{}'", name, value);
        }
        return src;
    }

    if ((first & 0xf0) == 0)  // literal without indexing
    {
        logger(kae::LogLevel::debug, "literal without indexing");
        uint64_t idx = consume_int(src, 4);
        if (idx) {  // indexed name
            std::string value = consume_str(src);
            logger(kae::LogLevel::debug, "{} = '{}'", idx, value);
        }
        else {
            std::string name = consume_str(src);
            std::string value = consume_str(src);
            logger(kae::LogLevel::debug, "'{}' = '{}'", name, value);
        }
        return src;
    }

    if ((first & 0xf0) == 0x10)  // literal never indexed
    {
        logger(kae::LogLevel::debug, "literal never indexed");
        uint64_t idx = consume_int(src, 4);
        if (idx) {  // indexed name
            std::string value = consume_str(src);
            logger(kae::LogLevel::debug, "{} = '{}'", idx, value);
        }
        else {
            std::string name = consume_str(src);
            std::string value = consume_str(src);
            logger(kae::LogLevel::debug, "'{}' = '{}'", name, value);
        }
        return src;
    }

    abort();
}

uint64_t Header::consume_int(kae::Span<const std::byte>& src, int prefix)
{
    assert(src.size() >= 1);
    uint8_t max_prefix = ~static_cast<uint8_t>(0xff << prefix);
    uint64_t value = utils::big_to_host(kae::bit_cast<uint8_t>(src[0])) & max_prefix;
    if (value != max_prefix) {
        src = src.subspan(1, src.size() - 1);
        return value;
    }

    size_t idx = 0;
    while (true) {
        ++idx;
        assert(src.size() > idx);
        auto next_byte = utils::big_to_host(kae::bit_cast<uint8_t>(src[idx]));
        value = value + (next_byte & 0x7f) * static_cast<uint64_t>(std::pow(2, (idx - 1) * 7));
        if ((next_byte & 0x80) != 0x80)
            break;
    }

    src = src.subspan(idx, src.size() - idx);
    return value;
}

std::string Header::consume_str(kae::Span<const std::byte>& src)
{
    bool huffman = utils::big_to_host(kae::bit_cast<uint8_t>(src[0])) & 0x80;
    uint64_t length = consume_int(src, 7);
    assert(src.size() >= length);
    std::string r;
    r.resize(length);
    std::transform(src.begin(), src.begin() + length, r.begin(), [&](std::byte b) { return static_cast<char>(b); });
    return r;
}


}  // namespace algue::http
