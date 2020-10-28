#pragma once

#include <string_view>

namespace algue::http::header_fields {

/// string_view proxy to allow implicit copy
struct HeaderField {
    constexpr HeaderField(const char* name) :
        name_{name}
    {}

    constexpr bool operator==(const HeaderField&) const = default;

    constexpr bool operator==(std::string_view other) const
    {
        return name_ == other;
    }

    constexpr bool operator==(const std::string& other) const
    {
        return name_ == other;
    }

    constexpr operator std::string_view() const
    {
        return name_;
    }

    operator std::string() const
    {
        return std::string{name_};
    }

private:
    std::string_view name_;
};


constexpr HeaderField accept = "accept";
constexpr HeaderField accept_charset = "accept-charset";
constexpr HeaderField accept_encoding = "accept-encoding";
constexpr HeaderField accept_language = "accept-language";
constexpr HeaderField accept_post = "accept-post";
constexpr HeaderField accept_ranges = "accept-ranges";
constexpr HeaderField age = "age";
constexpr HeaderField allow = "allow";
constexpr HeaderField alpn = "alpn";
constexpr HeaderField alt_svc = "alt-svc";
constexpr HeaderField alt_used = "alt-used";
constexpr HeaderField authentication_info = "authentication-info";
constexpr HeaderField authorization = "authorization";
constexpr HeaderField cache_control = "cache-control";
constexpr HeaderField cal_managed_id = "cal-managed-id";
constexpr HeaderField caldav_timezones = "caldav-timezones";
constexpr HeaderField cdn_loop = "cdn-loop";
constexpr HeaderField cert_not_after = "cert-not-after";
constexpr HeaderField cert_not_before = "cert-not-before";
constexpr HeaderField connection = "connection";
constexpr HeaderField content_disposition = "content-disposition";
constexpr HeaderField content_encoding = "content-encoding";
constexpr HeaderField content_language = "content-language";
constexpr HeaderField content_length = "content-length";
constexpr HeaderField content_location = "content-location";
constexpr HeaderField content_range = "content-range";
constexpr HeaderField content_type = "content-type";
constexpr HeaderField cookie = "cookie";
constexpr HeaderField dasl = "dasl";
constexpr HeaderField dav = "dav";
constexpr HeaderField date = "date";
constexpr HeaderField depth = "depth";
constexpr HeaderField destination = "destination";
constexpr HeaderField early_data = "early-data";
constexpr HeaderField etag = "etag";
constexpr HeaderField expect = "expect";
constexpr HeaderField expires = "expires";
constexpr HeaderField forwarded = "forwarded";
constexpr HeaderField from = "from";
constexpr HeaderField host = "host";
constexpr HeaderField http2_settings = "http2-settings";
constexpr HeaderField if_ = "if";
constexpr HeaderField if_match = "if-match";
constexpr HeaderField if_modified_since = "if-modified-since";
constexpr HeaderField if_none_match = "if-none-match";
constexpr HeaderField if_range = "if-range";
constexpr HeaderField if_schedule_tag_match = "if-schedule-tag-match";
constexpr HeaderField if_unmodified_since = "if-unmodified-since";
constexpr HeaderField include_referred_token_binding_id = "include-referred-token-binding-id";
constexpr HeaderField last_modified = "last-modified";
constexpr HeaderField link = "link";
constexpr HeaderField location = "location";
constexpr HeaderField lock_token = "lock-token";
constexpr HeaderField max_forwards = "max-forwards";
constexpr HeaderField mime_version = "mime-version";
constexpr HeaderField odata_entityid = "odata-entityid";
constexpr HeaderField odata_isolation = "odata-isolation";
constexpr HeaderField odata_maxversion = "odata-maxversion";
constexpr HeaderField odata_version = "odata-version";
constexpr HeaderField ordering_type = "ordering-type";
constexpr HeaderField origin = "origin";
constexpr HeaderField oscore = "oscore";
constexpr HeaderField overwrite = "overwrite";
constexpr HeaderField position = "position";
constexpr HeaderField pragma = "pragma";
constexpr HeaderField prefer = "prefer";
constexpr HeaderField preference_applied = "preference-applied";
constexpr HeaderField proxy_authenticate = "proxy-authenticate";
constexpr HeaderField proxy_authentication_info = "proxy-authentication-info";
constexpr HeaderField proxy_authorization = "proxy-authorization";
constexpr HeaderField public_key_pins = "public-key-pins";
constexpr HeaderField public_key_pins_report_only = "public-key-pins-report-only";
constexpr HeaderField range = "range";
constexpr HeaderField referer = "referer";
constexpr HeaderField replay_nonce = "replay-nonce";
constexpr HeaderField retry_after = "retry-after";
constexpr HeaderField schedule_reply = "schedule-reply";
constexpr HeaderField schedule_tag = "schedule-tag";
constexpr HeaderField sec_token_binding = "sec-token-binding";
constexpr HeaderField sec_websocket_accept = "sec-websocket-accept";
constexpr HeaderField sec_websocket_extensions = "sec-websocket-extensions";
constexpr HeaderField sec_websocket_key = "sec-websocket-key";
constexpr HeaderField sec_websocket_protocol = "sec-websocket-protocol";
constexpr HeaderField sec_websocket_version = "sec-websocket-version";
constexpr HeaderField server = "server";
constexpr HeaderField set_cookie = "set-cookie";
constexpr HeaderField slug = "slug";
constexpr HeaderField strict_transport_security = "strict-transport-security";
constexpr HeaderField te = "te";
constexpr HeaderField timeout = "timeout";
constexpr HeaderField topic = "topic";
constexpr HeaderField trailer = "trailer";
constexpr HeaderField transfer_encoding = "transfer-encoding";
constexpr HeaderField ttl = "ttl";
constexpr HeaderField urgency = "urgency";
constexpr HeaderField upgrade = "upgrade";
constexpr HeaderField user_agent = "user-agent";
constexpr HeaderField vary = "vary";
constexpr HeaderField via = "via";
constexpr HeaderField www_authenticate = "www-authenticate";
constexpr HeaderField warning = "warning";
constexpr HeaderField x_content_type_options = "x-content-type-options";

}  // namespace algue::http::header_fields
