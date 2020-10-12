#pragma once

#include <variant>

#include "json/types.h"
#include "utils/ptr.h"

namespace algue::json {

struct Value {
    Value(Null _ = {});
    Value(Boolean boolean);
    Value(Number number);
    Value(String string);
    Value(Array array);
    Value(Object object);

    Value& operator=(Null _);
    Value& operator=(Boolean boolean);
    Value& operator=(Number number);
    Value& operator=(String string);
    Value& operator=(Array array);
    Value& operator=(Object object);

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
    // containers of incomplete types are implementation-defined, so we add an indirection
    std::variant<Null,
                 Boolean,
                 Number,
                 String,
                 utils::Ptr<Array>,
                 utils::Ptr<Object>>
            data_;
};

}  // namespace algue::json
