#include "json/value.h"

#include <kae/exception.h>
#include <kae/pp.h>

namespace algue::json {

Type Value::type() const
{
    if (std::holds_alternative<Null>(data_))
        return Type::null;
    else if (std::holds_alternative<Boolean>(data_))
        return Type::boolean;
    else if (std::holds_alternative<Number>(data_))
        return Type::number;
    else if (std::holds_alternative<String>(data_))
        return Type::string;
    else if (std::holds_alternative<Array>(data_))
        return Type::array;
    else if (std::holds_alternative<Object>(data_))
        return Type::object;

    throw MAKE_EXCEPTION("unknown value type");
}

const Boolean& Value::as_boolean() const
{
    return std::get<Boolean>(data_);
}
Boolean& Value::as_boolean()
{
    return std::get<Boolean>(data_);
}

const Number& Value::as_number() const
{
    return std::get<Number>(data_);
}
Number& Value::as_number()
{
    return std::get<Number>(data_);
}

const String& Value::as_string() const
{
    return std::get<String>(data_);
}
String& Value::as_string()
{
    return std::get<String>(data_);
}

const Array& Value::as_array() const
{
    return std::get<Array>(data_);
}
Array& Value::as_array()
{
    return std::get<Array>(data_);
}

const Object& Value::as_object() const
{
    return std::get<Object>(data_);
}
Object& Value::as_object()
{
    return std::get<Object>(data_);
}

}  // namespace algue::json
