#include "json/dump.h"

#include <algorithm>
#include <cmath>
#include <iterator>

#include <fmt/format.h>
#include <kae/exception.h>

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
        append(value.get<bool>() ? "true" : "false");
        break;
    case Type::signed_number:
        append(std::to_string(value.get<long long>()));
        break;
    case Type::unsigned_number:
        append(std::to_string(value.get<unsigned long long>()));
        break;
    case Type::floating_point: {
        double d = value.get<double>();
        if (std::isnan(d) || std::isinf(d))
            throw MAKE_EXCEPTION("json does not support NaN nor infinite floating-point values");
        // floating point to_chars is not yet implemented in libstdc++
        append(fmt::format("{}", d));
    } break;
    case Type::string:
        append("\"");
        append(value.as<std::string>());
        append("\"");
        break;
    case Type::array:
        append("[");
        for (const Value& v : value.as<Array>()) {
            append(dump(v));
            append(",");
        }
        r.pop_back();  // pop trailing comma
        append("]");
        break;
    case Type::object:
        append("{");
        for (const auto& [key, v] : value.as<Object>()) {
            append("\"");
            append(key);
            append("\"");
            append(":");
            append(dump(v));
            append(",");
        }
        r.pop_back();  // pop trailing comma
        append("}");
        break;
    }

    return r;
}

}  // namespace algue::json
