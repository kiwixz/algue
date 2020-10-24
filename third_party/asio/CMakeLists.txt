cmake_minimum_required(VERSION 3.12)
include("kae/third_party")

third_party("asio" REPO "chriskohlhoff/asio" TAG "asio-1-18-0" DONT_RUN_CMAKE)

project("asio" CXX)

FetchContent_GetProperties("asio")
add_library("asio" EXCLUDE_FROM_ALL
    "${asio_SOURCE_DIR}/asio/src/asio.cpp"
    "${asio_SOURCE_DIR}/asio/src/asio_ssl.cpp"
)
add_library("asio::asio" ALIAS "asio")

target_compile_definitions("asio" PUBLIC "ASIO_SEPARATE_COMPILATION")
target_include_directories("asio" SYSTEM PUBLIC "${asio_SOURCE_DIR}/asio/include")
target_link_libraries("asio" PUBLIC "LibreSSL::SSL")
