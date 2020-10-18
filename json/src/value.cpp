#include "json/value.h"

#include <kae/exception.h>

namespace algue::json {

Type Value::type() const
{
    if (data_.valueless_by_exception())
        throw MAKE_EXCEPTION("value has no value due to an exception");

    return static_cast<Type>(data_.index());
}

}  // namespace algue::json
