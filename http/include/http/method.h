#pragma once

#include <string_view>

namespace algue::http::method {

constexpr std::string_view connect = "CONNECT";
constexpr std::string_view delete_ = "DELETE";
constexpr std::string_view get = "GET";
constexpr std::string_view head = "HEAD";
constexpr std::string_view options = "OPTIONS";
constexpr std::string_view patch = "PATCH";
constexpr std::string_view post = "POST";
constexpr std::string_view put = "PUT";
constexpr std::string_view trace = "TRACE";

}  // namespace algue::http::method
