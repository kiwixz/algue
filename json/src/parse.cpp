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

            char c = ctx.consume();
            switch (c) {
            case '"':
                r.push_back('"');
                break;
            case '\\':
                r.push_back('\\');
                break;
            case '/':
                r.push_back('/');
                break;
            case 'b':
                r.push_back('\b');
                break;
            case 'f':
                r.push_back('\f');
                break;
            case 'n':
                r.push_back('\n');
                break;
            case 'r':
                r.push_back('\r');
                break;
            case 't':
                r.push_back('\t');
                break;
            case 'u':
                throw MAKE_EXCEPTION("unicode-escaping is not implemented");
            default:
                throw MAKE_EXCEPTION("blackslash followed by invalid escape char '{}'", c);
            }

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
    auto parse_digits = [&] {
        if (ctx.ended())
            throw MAKE_EXCEPTION("end of input during parsing of digits");

        if (!(ctx.peek() >= '0' && ctx.peek() <= '9'))
            throw MAKE_EXCEPTION("expected a digit, got '{}'", ctx.peek());

        unsigned long long r = 0;
        do {
            unsigned long long new_r = r * 10 + ctx.consume() - '0';
            if (new_r < r)
                throw MAKE_EXCEPTION("overflow triggered when parsing a number");
            r = new_r;
        } while (!ctx.ended() && ctx.peek() >= '0' && ctx.peek() <= '9');

        return r;
    };

    bool negative = ctx.try_consume('-');
    unsigned long long integral = 0;
    if (!ctx.try_consume('0')) {
        integral = parse_digits();
    }

    if (ctx.try_consume('.')) {  // floating point
        size_t fractional_digits = ctx.remaining().size();
        unsigned long long fractional = parse_digits();
        fractional_digits -= ctx.remaining().size();

        double r = (static_cast<double>(integral)
                    + static_cast<double>(fractional) / std::pow(10, fractional_digits))
                   * (negative ? -1 : 1);

        if (ctx.try_consume('e') || ctx.try_consume('E')) {
            bool exponent_negative = ctx.try_consume('-');
            if (!exponent_negative) {
                ctx.try_consume('+');
            }
            int exponent = static_cast<int>(parse_digits());
            r *= std::pow(10, exponent_negative ? -exponent : exponent);
        }

        return r;
    }

    if (ctx.try_consume('e') || ctx.try_consume('E')) {
        bool exponent_negative = ctx.try_consume('-');
        if (!exponent_negative) {
            ctx.try_consume('+');
        }
        int exponent = static_cast<int>(parse_digits());
        integral *= static_cast<unsigned long long>(std::pow(10, exponent_negative ? -exponent : exponent));
    }

    if (negative) {
        if (integral > static_cast<unsigned long long>(-std::numeric_limits<long long>::min()))
            throw MAKE_EXCEPTION("overflow triggered when applying sign to number");
        return static_cast<long long>(-integral);
    }
    else
        return integral;
}

}  // namespace

Value parse(std::string_view input)
{
    Value root;
    std::vector<Value*> path;

    sax_parse(input, [&](ParseEvent event,
                         std::optional<std::string> key,
                         Value value) {
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
            auto r = parent.as<Object>().try_emplace(*key, std::move(value));
            if (!r.second)
                throw MAKE_EXCEPTION("duplicate key '{}' in object", *key);
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

void sax_parse(std::string_view input,
               kae::FunctionRef<ParseOperation(ParseEvent event,
                                               std::optional<std::string> key,
                                               Value value)>
                       callback)
{
    utils::ParsingContext ctx{input};

    auto skip_ws = [&] {
        return ctx.skip(" \r\n\t");
    };

    std::vector<Type> stack;

    do {
        skip_ws();
        if (ctx.ended())
            throw MAKE_EXCEPTION("input ended unexpectedly");

        if (ctx.try_consume(']')) {
            if (stack.empty() || stack.back() != Type::array)
                throw MAKE_EXCEPTION("mismatched '[' and ']'");
            stack.pop_back();
            callback(ParseEvent::end_array, {}, {});
        }
        else if (ctx.try_consume('}')) {
            if (stack.empty() || stack.back() != Type::object)
                throw MAKE_EXCEPTION("mismatched '[' and ']'");
            stack.pop_back();
            callback(ParseEvent::end_object, {}, {});
        }
        else {
            std::optional<std::string> key;
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
                callback(ParseEvent::begin_array, std::move(key), Array{});
                stack.push_back(Type::array);
            }
            else if (ctx.try_consume('{')) {
                callback(ParseEvent::begin_object, std::move(key), Object{});
                stack.push_back(Type::object);
            }
            else if (ctx.try_consume("null")) {
                callback(ParseEvent::value, std::move(key), null);
            }
            else if (ctx.try_consume("true")) {
                callback(ParseEvent::value, std::move(key), true);
            }
            else if (ctx.try_consume("false")) {
                callback(ParseEvent::value, std::move(key), false);
            }
            else if (ctx.peek() == '-' || (ctx.peek() >= '0' && ctx.peek() <= '9')) {
                callback(ParseEvent::value, std::move(key), parse_number(ctx));
            }
            else if (ctx.peek() == '"') {
                callback(ParseEvent::value, std::move(key), parse_string(ctx));
            }
            else
                throw MAKE_EXCEPTION("could not parse value");
        }

        skip_ws();
        ctx.try_consume(',');
    } while (!stack.empty());

    skip_ws();
    if (!ctx.ended())
        throw MAKE_EXCEPTION("finished parsing but got extra input");
}

}  // namespace algue::json
