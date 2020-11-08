#pragma once

#include <string_view>

#include <asio/ip/basic_endpoint.hpp>
#include <fmt/format.h>

template <typename Protocol>
struct fmt::formatter<asio::ip::basic_endpoint<Protocol>> : fmt::formatter<std::string_view> {
    template <typename FormatContext>
    auto format(const asio::ip::basic_endpoint<Protocol>& ep, FormatContext& ctx)
    {
        return format_to(ctx.out(), "{}:{}", ep.address().to_string(), ep.port());
    }
};
