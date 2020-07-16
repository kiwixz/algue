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

    auto idx = utils::big_to_host(kae::bit_cast<uint8_t>(src[0]));

    if (idx & 0x80)  // fully indexed
    {
        logger(kae::LogLevel::debug, "fully indexed");
        return src.subspan(1, src.size() - 1);
    }
}


}  // namespace algue::http
