#include "utils/bytes.h"

namespace algue::utils {

BytesWriter::BytesWriter(kae::Span<std::byte> data) :
    data_{data}
{}

bool BytesWriter::finished() const
{
    return index_ == data_.size();
}

size_t BytesWriter::written() const
{
    return index_;
}

kae::Span<std::byte> BytesWriter::put_zero(size_t n)
{
    index_ += n;
    return data_.subspan(index_ - n, n);
}

void BytesWriter::put(kae::Span<const std::byte> data)
{
    std::copy(data.begin(), data.end(), data_.begin() + index_);
    index_ += data.size();
}

void BytesWriter::put(kae::Span<const char> data)
{
    return put(data.as_bytes());
}

void BytesWriter::put(kae::Span<const unsigned char> data)
{
    return put(data.as_bytes());
}

void BytesWriter::put(std::string_view data)
{
    return put(kae::Span<const char>{data});
}

void BytesWriter::put(const std::string& data)
{
    return put(kae::Span<const char>{data});
}

void BytesWriter::put(const char* data)
{
    return put(std::string_view{data});
}


IndexSpan<Bytes> Bytes::append_zero(size_t n)
{
    size_t idx = size();
    insert(end(), n, {});
    return {this, idx, n};
}

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
