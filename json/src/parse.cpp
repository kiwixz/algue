#include "json/parse.h"

#include <kae/exception.h>

#include "utils/parsing_context.h"

namespace algue::json {

namespace {

std::string parse_string(utils::ParsingContext& ctx)
{
    if (!ctx.try_consume("\""))
        throw MAKE_EXCEPTION("expected quote for string, got '{}'", ctx.peek());

    std::string r;
    while (!ctx.ended()) {
        if (ctx.try_consume("\""))
            return r;

        if (ctx.try_consume("\\")) {
            if (ctx.ended())
                throw MAKE_EXCEPTION("end of input after a backslash");

            if (ctx.try_consume("\"")) {
                r.push_back('\"');
            }
            else if (ctx.try_consume("\\")) {
                r.push_back('\\');
            }
            else if (ctx.try_consume("/")) {
                r.push_back('/');
            }
            else if (ctx.try_consume("b")) {
                r.push_back('\b');
            }
            else if (ctx.try_consume("f")) {
                r.push_back('\f');
            }
            else if (ctx.try_consume("n")) {
                r.push_back('\n');
            }
            else if (ctx.try_consume("r")) {
                r.push_back('\r');
            }
            else if (ctx.try_consume("t")) {
                r.push_back('\t');
            }
            else if (ctx.try_consume("u"))
                throw MAKE_EXCEPTION("unicode-escaping is not implemented");
            else
                throw MAKE_EXCEPTION("blackslash followed by invalid escape char '{}'", ctx.peek());

            continue;
        }

        char c = ctx.consume();
        if (c < ' ')
            throw MAKE_EXCEPTION("got control char {:#x} during parsing of string", c);
        r.push_back(c);
    }

    throw MAKE_EXCEPTION("end of input during the parsing of a string");
}

Value parse_number(utils::ParsingContext& ctx)
{
    while (!ctx.ended() && (ctx.peek() == '-' || (ctx.peek() >= '0' && ctx.peek() <= '9'))) {
        ctx.consume();
    }

    return 0;
}

}  // namespace

Value parse(std::string_view input)
{
    Value root;
    std::vector<Value*> path;

    parse(input, [&](ParseEvent event, std::string key, Value value) {
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
            auto r = parent.as<Object>().try_emplace(key, std::move(value));
            if (!r.second) {
                throw MAKE_EXCEPTION("duplicate key '{}' in object", key);
            }
            if (event == ParseEvent::begin_object || event == ParseEvent::begin_array) {
                path.push_back(&r.first->second);
            }
        }
        else {
            parent.as<Array>().push_back(std::move(value));
            if (event == ParseEvent::begin_object || event == ParseEvent::begin_array) {
                path.push_back(&parent.as<Array>().back());
            }
        }

        return ParseOperation::parse;
    });

    return root;
}

void parse(std::string_view input,
           kae::FunctionRef<ParseOperation(ParseEvent event, std::string key, Value value)>
                   callback)
{
    utils::ParsingContext ctx{input};

    auto skip_ws = [&] {
        return ctx.skip(" \r\n\t");
    };

    std::vector<Type> stack;

    while (true) {
        skip_ws();
        if (ctx.ended())
            throw MAKE_EXCEPTION("input ended unexpectedly");

        if (ctx.try_consume(']')) {
            if (stack.back() != Type::array)
                throw MAKE_EXCEPTION("mismatched '[' and ']'");
            stack.pop_back();
            callback(ParseEvent::end_array, {}, {});
        }
        else if (ctx.try_consume('}')) {
            if (stack.back() != Type::object)
                throw MAKE_EXCEPTION("mismatched '[' and ']'");
            stack.pop_back();
            callback(ParseEvent::end_object, {}, {});
        }
        else {
            std::string key;
            if (!stack.empty() && stack.back() == Type::object) {
                skip_ws();
                if (ctx.ended())
                    throw MAKE_EXCEPTION("expected key but input ended");
                key = parse_string(ctx);

                skip_ws();
                if (ctx.ended())
                    throw MAKE_EXCEPTION("expected colon but input ended");
                else if (!ctx.try_consume(":"))
                    throw MAKE_EXCEPTION("expected colon, got '{}'", ctx.peek());
            }

            skip_ws();
            if (ctx.ended())
                throw MAKE_EXCEPTION("expected value but input ended");

            if (ctx.try_consume('[')) {
                callback(ParseEvent::begin_array, key, Array{});
                stack.push_back(Type::array);
            }
            else if (ctx.try_consume('{')) {
                callback(ParseEvent::begin_object, key, Object{});
                stack.push_back(Type::object);
            }
            else if (ctx.try_consume("null")) {
                callback(ParseEvent::value, key, null);
            }
            else if (ctx.try_consume("true")) {
                callback(ParseEvent::value, key, true);
            }
            else if (ctx.try_consume("false")) {
                callback(ParseEvent::value, key, false);
            }
            else if (ctx.peek() == '-' || (ctx.peek() >= '0' && ctx.peek() <= '9')) {
                callback(ParseEvent::value, key, parse_number(ctx));
            }
            else if (ctx.peek() == '"') {
                callback(ParseEvent::value, key, parse_string(ctx));
            }
            else
                throw MAKE_EXCEPTION("could not parse value");
        }

        skip_ws();
        ctx.try_consume(',');

        if (stack.empty())
            break;
    }

    skip_ws();
    if (!ctx.ended())
        throw MAKE_EXCEPTION("finished parsing but got extra input");
}

}  // namespace algue::json
