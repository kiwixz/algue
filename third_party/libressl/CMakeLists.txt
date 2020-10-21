cmake_minimum_required(VERSION 3.12)
include("kae/third_party")

set(LIBRESSL_SKIP_INSTALL ON CACHE BOOL "")
set(LIBRESSL_APPS OFF CACHE BOOL "")
set(LIBRESSL_TESTS OFF CACHE BOOL "")

third_party("libressl"
    URL "https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-3.2.2.tar.gz"
    PATCHES
        "000-fix_cmake.patch"
        "001-enable_clang_attributes.patch"
)

add_library("LibreSSL::Crypto" ALIAS "crypto")
add_library("LibreSSL::SSL" ALIAS "ssl")
add_library("LibreSSL::TLS" ALIAS "tls")
