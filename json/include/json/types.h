#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace algue::json {

enum class Type {
    null,
    boolean,
    number,
    string,
    array,
    object,
};

struct Value;

struct Null {};
using Boolean = bool;
using Number = unsigned long long;
using String = std::string;
using Array = std::vector<Value>;
using Object = std::unordered_map<String, Value>;

inline constexpr Null null;

}  // namespace algue::json
