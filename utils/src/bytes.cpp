#include "utils/bytes.h"

namespace algue::utils {

Bytes::iterator Bytes::append(kae::Span<const std::byte> data)
{
    return insert(end(), data.begin(), data.end());
}

Bytes::iterator Bytes::append(kae::Span<const char> data)
{
    return append(data.as_bytes());
}

Bytes::iterator Bytes::append(kae::Span<const unsigned char> data)
{
    return append(data.as_bytes());
}

Bytes::iterator Bytes::append(std::string_view data)
{
    return append(kae::Span<const char>{data});
}

Bytes::iterator Bytes::append(const std::string& data)
{
    return append(kae::Span<const char>{data});
}

Bytes::iterator Bytes::append(const char* data)
{
    return append(std::string_view{data});
}

}  // namespace algue::utils
