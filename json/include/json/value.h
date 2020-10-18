#pragma once

#include <variant>

#include <kae/exception.h>

#include "json/types.h"
#include "utils/always_false.h"
#include "utils/value_ptr.h"

namespace algue::json {

struct Value {
    Value() = default;

    template <typename T>
    Value(T data)
    {
        if constexpr (std::is_same_v<T, Null>)
            data_ = null;
        else if constexpr (std::is_same_v<T, bool>)
            data_ = data;
        else if constexpr (std::is_integral_v<T>) {
            if constexpr (std::is_signed_v<T>)
                data_ = static_cast<long long>(data);
            else
                data_ = static_cast<unsigned long long>(data);
        }
        else if constexpr (std::is_floating_point_v<T>)
            data_ = static_cast<double>(data);
        else if constexpr (std::is_same_v<T, std::string>)
            data_ = std::move(data);
        else if constexpr (std::is_same_v<T, const char*>)
            data_ = std::string{data};
        else if constexpr (std::is_same_v<T, Array>)
            data_ = utils::ValuePtr<Array>{std::move(data)};
        else if constexpr (std::is_same_v<T, Object>)
            data_ = utils::ValuePtr<Object>{std::move(data)};
        else
            static_assert(utils::always_false<T>, "unrecognized json type");
    }

    bool operator==(const Value&) const = default;

    Type type() const;

    template <typename T>
    const T& as() const
    {
        if constexpr (std::is_same_v<T, std::string>)
            return std::get<std::string>(data_);
        else if constexpr (std::is_same_v<T, Array>)
            return *std::get<utils::ValuePtr<Array>>(data_);
        else if constexpr (std::is_same_v<T, Object>)
            return *std::get<utils::ValuePtr<Object>>(data_);
        else
            static_assert(utils::always_false<T>, "this type cant be accessed as a reference");
    }

    template <typename T>
    T& as()
    {
        return const_cast<T&>(const_cast<const Value&>(*this).as<T>());
    }

    template <typename T>
    T get() const
    {
        if constexpr (std::is_same_v<T, Null>)
            return std::get<Null>(data_);
        else if constexpr (std::is_same_v<T, bool>)
            return std::get<bool>(data_);
        else if constexpr (std::is_integral_v<T>) {
            if (std::holds_alternative<long long>(data_)) {
                long long r = std::get<long long>(data_);
                if (r < static_cast<long long>(std::numeric_limits<T>::min())
                    || (r > 0 && static_cast<unsigned long long>(r) > static_cast<unsigned long long>(std::numeric_limits<T>::max())))
                    throw MAKE_EXCEPTION("current signed value cannot be represented with the given type");
                return static_cast<T>(r);
            }
            else {
                unsigned long long r = std::get<unsigned long long>(data_);
                if (r > static_cast<unsigned long long>(std::numeric_limits<T>::max()))
                    throw MAKE_EXCEPTION("current unsigned value cannot be represented with the given type");
                return static_cast<T>(r);
            }
        }
        else if constexpr (std::is_floating_point_v<T>)
            return static_cast<T>(std::get<double>(data_));
        else if constexpr (std::is_same_v<T, std::string>)
            return std::get<std::string>(data_);
        else if constexpr (std::is_same_v<T, Array>)
            return *std::get<utils::ValuePtr<Array>>(data_);
        else if constexpr (std::is_same_v<T, Object>)
            return *std::get<utils::ValuePtr<Object>>(data_);
        else
            static_assert(utils::always_false<T>, "unrecognized json type");
    }

private:
    // containers of incomplete types are implementation-defined, so we add an indirection
    std::variant<Null,
                 bool,
                 long long,
                 unsigned long long,
                 double,
                 std::string,
                 utils::ValuePtr<Array>,
                 utils::ValuePtr<Object>>
            data_;
};

}  // namespace algue::json
