#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "json/parse.h"
#include "json/value.h"

namespace algue::json {

struct DumpIteration {
    ParseEvent event;
    std::optional<std::string_view> key;
    const Value* value;
};

/// Spacing can be negative for maximum density, or an indentation width.
std::string dump(const Value& root, int spacing = -1);

/// Use semantics compatible with the parse() function
std::string sax_dump(kae::FunctionRef<DumpIteration()> callback, int spacing = -1);

std::string format(std::string_view input, int spacing);

}  // namespace algue::json
