#pragma once

#include <algorithm>
#include <string_view>

namespace algue::utils {

constexpr bool iequal(std::string_view a, std::string_view b)
{
    return a.size() == b.size()
           && std::equal(a.begin(), a.end(), b.begin(), [](char ac, char bc) {
                  auto to_lower = [](char c) { return c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c; };
                  return to_lower(ac) == to_lower(bc);
              });
}

}  // namespace algue::utils
