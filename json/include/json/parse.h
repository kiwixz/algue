#pragma once

#include <string>
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


Value parse(std::string_view input);

/// Will call the callback during JSON parsing.
/// Arguments will only match one of the following:
///     value, key if any, value
///     begin_*, key, empty array/object
///     end_*, undefined, undefined
void sax_parse(std::string_view input,
               kae::FunctionRef<ParseOperation(ParseEvent event, std::string key, Value value)>
                       callback);

}  // namespace algue::json
