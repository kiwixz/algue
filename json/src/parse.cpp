#include "json/parse.h"

#include <kae/exception.h>

namespace algue::json {

Value parse(std::string_view src)
{
    Value root;
    std::vector<Value*> path;

    parse(src, [&](ParseEvent event, std::string_view key, Value value) {
        if (event == ParseEvent::end_object || event == ParseEvent::end_array) {
            path.pop_back();
            return ParseOperation::parse;
        }

        if (path.empty()) {  // first call
            root = std::move(value);
            path.push_back(&root);
        }

        Value& parent = *path.back();
        if (parent.type() == Type::object) {
            auto r = parent.as_object().try_emplace(std::string{key}, std::move(value));
            if (!r.second) {
                throw MAKE_EXCEPTION("duplicate key '{}' in object", key);
            }
            if (event == ParseEvent::begin_object || event == ParseEvent::begin_array) {
                path.push_back(&r.first->second);
            }
        }
        else {
            parent.as_array().push_back(std::move(value));
            if (event == ParseEvent::begin_object || event == ParseEvent::begin_array) {
                path.push_back(&parent.as_array().back());
            }
        }

        return ParseOperation::parse;
    });

    return root;
}

void parse(std::string_view src,
           kae::FunctionRef<ParseOperation(ParseEvent event, std::string_view key, Value value)>
                   callback)
{}

}  // namespace algue::json
