cmake_minimum_required(VERSION 3.13)
include("algue/cpp_test")

set(target "http")

file(GLOB_RECURSE src CONFIGURE_DEPENDS "src/*.cpp")
add_library("${target}" ${src})
target_include_directories("${target}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (ALGUE_TESTS)
   file(GLOB_RECURSE test_src CONFIGURE_DEPENDS "test/*.cpp")
   cpp_test("${target}" ${test_src})
endif ()

file(DOWNLOAD "https://curl.haxx.se/ca/cacert.pem" "${CMAKE_BINARY_DIR}/data/cacert.pem")


target_link_libraries("${target}" PUBLIC
    "kae::kae"
    "asio::asio"

    "utils"
)
