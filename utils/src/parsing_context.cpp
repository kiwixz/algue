#include "utils/parsing_context.h"

#include <kae/pp.h>

namespace algue::utils {

ParsingContext::ParsingContext(std::string_view input) :
    remaining_{input}
{}

bool ParsingContext::ended() const
{
    return remaining_.empty();
}

std::string_view ParsingContext::remaining() const
{
    return remaining_;
}

char ParsingContext::peek() const
{
    ASSERT(!remaining_.empty());
    return remaining_[0];
}

bool ParsingContext::try_consume(char c)
{
    if (!remaining_.starts_with(c))
        return false;

    remaining_.remove_prefix(1);
    return true;
}

bool ParsingContext::try_consume(std::string_view prefix)
{
    if (!remaining_.starts_with(prefix))
        return false;

    remaining_.remove_prefix(prefix.size());
    return true;
}

std::optional<std::string_view> ParsingContext::try_consume_until(char sentinel)
{
    size_t i = remaining_.find(sentinel);
    if (i == std::string_view::npos)
        return {};

    std::string_view r = consume(i);
    remaining_.remove_prefix(1);
    return r;
}

std::optional<std::string_view> ParsingContext::try_consume_until(std::string_view sentinel)
{
    size_t i = remaining_.find(sentinel);
    if (i == std::string_view::npos)
        return {};

    std::string_view r = consume(i);
    remaining_.remove_prefix(sentinel.size());
    return r;
}

char ParsingContext::consume()
{
    ASSERT(!remaining_.empty());
    char c = remaining_[0];
    remaining_.remove_prefix(1);
    return c;
}

std::string_view ParsingContext::consume(size_t length)
{
    ASSERT(remaining_.size() >= length);
    std::string_view r = remaining_.substr(0, length);
    remaining_.remove_prefix(length);
    return r;
}

size_t ParsingContext::skip(char c)
{
    size_t n = std::min(remaining_.find_first_not_of(c), remaining_.size());
    remaining_.remove_prefix(n);
    return n;
}

size_t ParsingContext::skip(std::string_view chars)
{
    size_t n = std::min(remaining_.find_first_not_of(chars), remaining_.size());
    remaining_.remove_prefix(n);
    return n;
}

size_t ParsingContext::skip(const char* chars)
{
    return skip(std::string_view{chars});
}

size_t ParsingContext::skip(kae::FunctionRef<bool(char)> predicate)
{
    auto it = std::find_if_not(remaining_.begin(), remaining_.end(), predicate);
    size_t n = (it == remaining_.end() ? remaining_.size() : it - remaining_.begin());
    remaining_.remove_prefix(n);
    return n;
}

}  // namespace algue::utils
