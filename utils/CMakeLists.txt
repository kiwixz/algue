cmake_minimum_required(VERSION 3.13)
include("algue/cpp_test")

set(target "utils")

file(GLOB_RECURSE src CONFIGURE_DEPENDS "src/*.cpp")
add_library("${target}" ${src})
target_include_directories("${target}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if (ALGUE_TESTS)
    file(GLOB_RECURSE test_src CONFIGURE_DEPENDS "test/*.cpp")
    cpp_test("${target}" ${test_src})
endif ()


target_link_libraries("${target}" PUBLIC
    "kae::kae"
)
