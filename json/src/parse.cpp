#include "json/parse.h"

#include <kae/exception.h>

namespace algue::json {

Value parse(std::string_view src)
{
    Value root;
    std::vector<Value*> path;

    parse(src, [&](ParseEvent event, std::string key, Value value) {
        if (event == ParseEvent::end_object || event == ParseEvent::end_array) {
            path.pop_back();
            return ParseOperation::parse;
        }

        if (path.empty()) {  // first call
            root = std::move(value);
            path.push_back(&root);
            return ParseOperation::parse;
        }

        Value& parent = *path.back();
        if (parent.type() == Type::object) {
            auto r = parent.as_object().try_emplace(key, std::move(value));
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
           kae::FunctionRef<ParseOperation(ParseEvent event, std::string key, Value value)>
                   callback)
{
    std::string_view rem = src;

    auto try_consume = [&](std::string_view token) {
        if (rem.substr(0, token.size()) != token)
            return false;

        rem.remove_prefix(token.size());
        return true;
    };

    auto skip_ws = [&] {
        rem.remove_prefix(std::min(rem.find_first_not_of(" \r\n\t"), rem.size()));
    };

    auto parse_string = [&] {
        if (!try_consume("\""))
            throw MAKE_EXCEPTION("expected quote for string, got '{}'", rem[0]);

        std::string r;
        for (size_t i = 0; i < rem.size(); ++i) {
            char c = rem[i];

            if (c == '"') {
                int backslashes = 0;
                for (size_t j = 1; j <= i; ++j) {
                    if (rem[i - j] != '\\')
                        break;
                    ++backslashes;
                }
                if (backslashes % 2 == 0) {  // this ensures its not an escaped quote (\", \\\", \\\\\", ...)
                    rem.remove_prefix(i + 1);
                    return r;
                }
            }

            if (c < ' ')
                throw MAKE_EXCEPTION("got control char {:#x} during parsing of string", c);

            r.push_back(c);
        }

        throw MAKE_EXCEPTION("end of input during the parsing of a string");
    };


    std::vector<Type> stack;

    while (true) {
        if (try_consume("]")) {
            if (stack.back() != Type::array)
                throw MAKE_EXCEPTION("mismatched '[' and ']'");
            stack.pop_back();
            callback(ParseEvent::end_array, {}, {});
        }
        else if (try_consume("}")) {
            if (stack.back() != Type::object)
                throw MAKE_EXCEPTION("mismatched '[' and ']'");
            stack.pop_back();
            callback(ParseEvent::end_object, {}, {});
        }
        else {
            std::string key;
            if (!stack.empty() && stack.back() == Type::object) {
                skip_ws();
                if (rem.empty())
                    throw MAKE_EXCEPTION("expected key but input ended");
                key = parse_string();

                skip_ws();
                if (rem.empty())
                    throw MAKE_EXCEPTION("expected colon but input ended");
                else if (!try_consume(":"))
                    throw MAKE_EXCEPTION("expected colon, got '{}'", rem[0]);
            }

            skip_ws();
            if (rem.empty())
                throw MAKE_EXCEPTION("expected value but input ended");

            if (try_consume("[")) {
                callback(ParseEvent::begin_array, key, Array{});
                stack.push_back(Type::array);
            }
            else if (try_consume("{")) {
                callback(ParseEvent::begin_object, key, Object{});
                stack.push_back(Type::object);
            }
            else if (try_consume("null"))
                callback(ParseEvent::value, key, null);
            else if (try_consume("true"))
                callback(ParseEvent::value, key, true);
            else if (try_consume("false"))
                callback(ParseEvent::value, key, false);
            else if (rem[0] == '-' || (rem[0] >= '0' && rem[0] <= '9')) {
                while (rem[0] == '-' || (rem[0] >= '0' && rem[0] <= '9')) {
                    rem.remove_prefix(1);
                }
                callback(ParseEvent::value, key, 0ull);
            }
            else if (rem[0] == '"') {
                callback(ParseEvent::value, key, parse_string());
            }
            else {
                throw MAKE_EXCEPTION("could not parse value");
            }
        }

        skip_ws();
        try_consume(",");

        if (stack.empty())
            break;
    }

    skip_ws();
    if (!rem.empty())
        throw MAKE_EXCEPTION("finished parsing but got extra input");
}

}  // namespace algue::json
