#include "json/value.h"

#include <kae/exception.h>

namespace algue::json {

const json::Value& Value::operator[](size_t index) const
{
    return as<Array>()[index];
}

json::Value& Value::operator[](size_t index)
{
    return as<Array>()[index];
}

const json::Value& Value::operator[](const std::string& key) const
{
    return as<Object>().at(key);
}

json::Value& Value::operator[](const std::string& key)
{
    return as<Object>().at(key);
}

Type Value::type() const
{
    if (data_.valueless_by_exception())
        throw MAKE_EXCEPTION("value has no value due to an exception");

    return static_cast<Type>(data_.index());
}

}  // namespace algue::json
