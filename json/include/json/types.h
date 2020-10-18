#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace algue::json {

enum class Type {
    null,
    boolean,
    signed_number,
    unsigned_number,
    floating_point,
    string,
    array,
    object,
};

struct Value;

struct Null {};
using Array = std::vector<Value>;
using Object = std::unordered_map<std::string, Value>;

inline constexpr Null null;

}  // namespace algue::json
