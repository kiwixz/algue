#pragma once

namespace algue::utils {

template <typename... TBases>
struct Overloaded : TBases... {
    using TBases::operator()...;
};

template <typename... TBases>
Overloaded(TBases...) -> Overloaded<TBases...>;

}  // namespace algue::utils
