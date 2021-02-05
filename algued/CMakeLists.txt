cmake_minimum_required(VERSION 3.12)

set(target "algued")

file(GLOB_RECURSE src CONFIGURE_DEPENDS "src/*.cpp")
list(FILTER src EXCLUDE REGEX "/main\.cpp$")
add_library("${target}_" ${src})
target_include_directories("${target}_" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (ALGUE_TESTS)
    file(GLOB_RECURSE test_src CONFIGURE_DEPENDS "test/*.cpp")
    cpp_test("${target}_" ${test_src})
endif ()

add_executable("${target}" "src/main.cpp")
target_link_libraries("${target}" PUBLIC "${target}_")


target_link_libraries("${target}_" PUBLIC
    "kae::kae"
    "asio::asio"
    "sqlite::sqlite"

    "http"
    "json"
    "utils"
)
