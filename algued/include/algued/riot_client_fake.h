#pragma once

#include <optional>
#include <string_view>

namespace algue::algued {

std::optional<std::string_view> get_riot_client_fake(std::string_view url);

}  // namespace algue::algued
