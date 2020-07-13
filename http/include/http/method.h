#pragma once

#include <string_view>

namespace algue::http {

enum class Method {
    get
};

constexpr std::string_view to_string(Method method)
{
    switch (method) {
    case Method::get:
        return "GET";
    }
}

}  // namespace algue::http
