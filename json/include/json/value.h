#pragma once

#include <memory>
#include <variant>

#include "json/types.h"

namespace algue::json {

struct Value {
    Value() = default;

    template <typename T>
    Value(T&& data) :
        data_{std::forward<T>(data)}
    {}

    template <typename T>
    Value& operator=(T&& data)
    {
        data_ = std::forward<T>(data);
        return *this;
    }

    Type type() const;

    const Boolean& as_boolean() const;
    Boolean& as_boolean();
    const Number& as_number() const;
    Number& as_number();
    const String& as_string() const;
    String& as_string();
    const Array& as_array() const;
    Array& as_array();
    const Object& as_object() const;
    Object& as_object();

private:
    std::variant<Null, Boolean, Number, String, Array, Object> data_;
};

}  // namespace algue::json
