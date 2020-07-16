#pragma once

#include <iterator>

#include <kae/function_ref.h>

namespace algue::utils {

template <typename I, typename T>
constexpr I lower_bound(I first, I last, const T& value)
{
    return lower_bound_if(first, last, [&](const decltype(*first)& v) { return v < value; });
}

template <typename C, typename T>
constexpr auto lower_bound(C& container, const T& value)
{
    // namespace to not get std version
    return utils::lower_bound(container.begin(), container.end(), value);
}

template <typename I>
constexpr I lower_bound_if(I first, I last, kae::FunctionRef<bool(const decltype(*first)&)> predicate)
{
    size_t count = last - first;
    while (count > 0) {
        size_t step = count / 2;
        I it = first + step;
        if (predicate(*it)) {
            first = it + 1;
            count -= step + 1;
        }
        else {
            count = step;
        }
    }
    return first;
}

template <typename C>
constexpr auto lower_bound_if(C& container, kae::FunctionRef<bool(const decltype(*container.begin())&)> predicate)
{
    // namespace to not get std version
    return lower_bound_if(container.begin(), container.end(), predicate);
}

}  // namespace algue::utils
