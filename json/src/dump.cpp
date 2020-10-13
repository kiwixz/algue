#include "json/dump.h"

#include <algorithm>
#include <iterator>

namespace algue::json {

std::string dump(Value value)
{
    std::string r;
    r.reserve(4096);

    auto append = [&](const auto& c) {
        if constexpr (std::is_same_v<std::decay_t<decltype(c)>, const char*>) {
            std::string_view str = c;
            std::copy(str.begin(), str.end(), std::back_inserter(r));
        }
        else {
            std::copy(c.begin(), c.end(), std::back_inserter(r));
        }
    };

    switch (value.type()) {
    case Type::null:
        append("null");
        break;
    case Type::boolean:
        append(value.as_boolean() ? "true" : "false");
        break;
    case Type::number:
        append(std::to_string(value.as_number()));
        break;
    case Type::string:
        append(value.as_string());
        break;
    case Type::array:
        for (const Value& v : value.as_array()) {
            append(dump(v));
        }
        break;
    case Type::object:
        for (const auto& [key, v] : value.as_object()) {
            append("\"");
            append(key);
            append("\"");
            append(":");
            append(dump(v));
        }
        break;
    }

    return r;
}

}  // namespace algue::json
