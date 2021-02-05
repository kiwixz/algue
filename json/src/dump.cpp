#include "json/dump.h"

#include <algorithm>
#include <cmath>
#include <iterator>

#include <fmt/format.h>
#include <kae/exception.h>

namespace algue::json {

std::string dump(const Value& root, int spacing)
{
    struct ArrayIt {
        Array::const_iterator current;
        Array::const_iterator end;
    };
    struct ObjectIt {
        Object::const_iterator current;
        Object::const_iterator end;
    };
    std::vector<std::variant<ArrayIt, ObjectIt>> stack;

    return sax_dump([&]() -> DumpIteration {
        std::optional<std::string_view> key;
        const Value* value = &root;
        if (!stack.empty()) {  // not the first call
            if (auto array_it = std::get_if<ArrayIt>(&stack.back())) {
                if (array_it->current == array_it->end) {
                    stack.pop_back();
                    return {ParseEvent::end_array, {}, {}};
                }
                value = &*array_it->current;
                ++array_it->current;
            }
            else if (auto object_it = std::get_if<ObjectIt>(&stack.back())) {
                if (object_it->current == object_it->end) {
                    stack.pop_back();
                    return {ParseEvent::end_object, {}, {}};
                }
                key = object_it->current->first;
                value = &object_it->current->second;
                ++object_it->current;
            }
        }

        switch (value->type()) {
        case Type::null:
        case Type::boolean:
        case Type::signed_number:
        case Type::unsigned_number:
        case Type::floating_point:
        case Type::string:
            return {ParseEvent::value, key, value};
        case Type::array:
            stack.emplace_back(ArrayIt{value->as<Array>().begin(), value->as<Array>().end()});
            return {ParseEvent::begin_array, key, {}};
        case Type::object:
            stack.emplace_back(ObjectIt{value->as<Object>().begin(), value->as<Object>().end()});
            return {ParseEvent::begin_object, key, {}};
        }
        std::abort();
    },
                    spacing);
}

std::string sax_dump(kae::FunctionRef<DumpIteration()> callback, int spacing)
{
    std::string r;
    r.reserve(4096);

    int depth = 0;
    bool comma = false;

    auto indent = [&] {
        if (spacing <= 0)
            return;

        if (!r.empty()) {
            r += '\n';
        }
        for (int i = 0; i < depth * spacing; ++i) {
            r += ' ';
        }
    };

    do {
        DumpIteration it = callback();

        if (it.event == ParseEvent::end_array) {
            --depth;
            indent();
            comma = true;
            r += ']';
            continue;
        }
        else if (it.event == ParseEvent::end_object) {
            --depth;
            indent();
            comma = true;
            r += '}';
            continue;
        }

        if (comma) {
            r += ',';
            if (spacing == 0) {
                r += ' ';
            }
        }
        indent();
        comma = true;

        if (it.key) {
            r += '"';
            r += *it.key;
            r += "\":";
            if (spacing >= 0) {
                r += ' ';
            }
        }

        switch (it.event) {
        case ParseEvent::value:
            switch (it.value->type()) {
            case Type::null:
                r += "null";
                break;
            case Type::boolean:
                r += it.value->get<bool>() ? "true" : "false";
                break;
            case Type::signed_number:
                r += std::to_string(it.value->get<long long>());
                break;
            case Type::unsigned_number:
                r += std::to_string(it.value->get<unsigned long long>());
                break;
            case Type::floating_point: {
                double d = it.value->get<double>();
                if (std::isnan(d) || std::isinf(d))
                    throw MAKE_EXCEPTION("json does not support NaN nor infinite floating-point values");
                // floating point to_chars is not yet implemented in libstdc++
                r += fmt::format("{}", d);
            } break;
            case Type::string:
                r += '"' + it.value->as<std::string>() + '"';
                break;
            case Type::array:
            case Type::object:
                ASSERT(false);
            }
            break;
        case ParseEvent::begin_array:
            r += '[';
            comma = false;
            ++depth;
            break;
        case ParseEvent::begin_object:
            r += '{';
            comma = false;
            ++depth;
            break;
        case ParseEvent::end_array:
        case ParseEvent::end_object:
            ASSERT(false);
        }
    } while (depth > 0);

    if (spacing > 0) {
        r += '\n';
    }
    return r;
}

std::string format(std::string_view input, int spacing)
{
    struct ParseIteration {
        ParseEvent event;
        std::optional<std::string> key;
        Value value;
    };
    std::vector<ParseIteration> stack;

    sax_parse(input, [&](ParseEvent event,
                         std::optional<std::string> key,
                         Value value) {
        stack.push_back({event, std::move(key), std::move(value)});
        return ParseOperation::parse;
    });

    auto it = stack.begin();
    return sax_dump([&]() {
        DumpIteration r{it->event, it->key, &it->value};
        ++it;
        return r;
    },
                    spacing);
}

}  // namespace algue::json
