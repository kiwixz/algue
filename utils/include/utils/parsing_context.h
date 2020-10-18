#pragma once

#include <string_view>

#include <kae/function_ref.h>

namespace algue::utils {

struct ParsingContext {
    explicit ParsingContext(std::string_view input);

    bool ended() const;
    std::string_view remaining() const;
    char peek() const;

    bool try_consume(char c);
    bool try_consume(std::string_view prefix);
    char consume();

    size_t skip(char c);
    size_t skip(std::string_view chars);
    size_t skip(kae::FunctionRef<bool(char)> predicate);

    // pointer would be ambiguous with FunctionRef
    size_t skip(const char* chars);

private:
    std::string_view remaining_;
};

}  // namespace algue::utils
