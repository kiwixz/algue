#include "json/value.h"

#include <kae/exception.h>
#include <kae/pp.h>

namespace algue::json {

Value::Value(Null /*_*/) :
    data_{null}
{}

Value::Value(Boolean boolean) :
    data_{boolean}
{}

Value::Value(Number number) :
    data_{number}
{}

Value::Value(String string) :
    data_{std::move(string)}
{}

Value::Value(Array array) :
    data_{utils::ValuePtr<Array>{std::move(array)}}
{}

Value::Value(Object object) :
    data_{utils::ValuePtr<Object>{std::move(object)}}
{}


Value& Value::operator=(Null /*_*/)
{
    data_ = null;
    return *this;
}

Value& Value::operator=(Boolean boolean)
{
    data_ = boolean;
    return *this;
}

Value& Value::operator=(Number number)
{
    data_ = number;
    return *this;
}

Value& Value::operator=(String string)
{
    data_ = std::move(string);
    return *this;
}

Value& Value::operator=(Array array)
{
    data_ = utils::ValuePtr<Array>{std::move(array)};
    return *this;
}

Value& Value::operator=(Object object)
{
    data_ = utils::ValuePtr<Object>{std::move(object)};
    return *this;
}


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
    else if (std::holds_alternative<utils::ValuePtr<Array>>(data_))
        return Type::array;
    else if (std::holds_alternative<utils::ValuePtr<Object>>(data_))
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
    return *std::get<utils::ValuePtr<Array>>(data_);
}
Array& Value::as_array()
{
    return *std::get<utils::ValuePtr<Array>>(data_);
}

const Object& Value::as_object() const
{
    return *std::get<utils::ValuePtr<Object>>(data_);
}
Object& Value::as_object()
{
    return *std::get<utils::ValuePtr<Object>>(data_);
}

}  // namespace algue::json
