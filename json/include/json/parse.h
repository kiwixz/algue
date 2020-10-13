#pragma once

#include <string_view>

#include <kae/function_ref.h>

#include "json/value.h"

namespace algue::json {

enum class ParseEvent {
    value,
    begin_array,
    end_array,
    begin_object,
    end_object,
};

enum class ParseOperation {
    parse,
    stop,
};


Value parse(std::string_view src);

void parse(std::string_view src,
           kae::FunctionRef<ParseOperation(ParseEvent event, std::string_view key, Value value)>
                   callback);

}  // namespace algue::json
