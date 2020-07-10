cmake_minimum_required(VERSION 3.12)
include("kae/cpp_test")

set(target "lcu_api")
set(target_lib "${target}_lib")

file(GLOB_RECURSE src CONFIGURE_DEPENDS "src/*.cpp")
list(FILTER src EXCLUDE REGEX "/main.cpp$")
add_library("${target_lib}" STATIC ${src})
target_include_directories("${target_lib}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable("${target}" "src/main.cpp")
target_link_libraries("${target}" PRIVATE "${target_lib}")

if (ALGUE_TESTS)
   file(GLOB_RECURSE test_src CONFIGURE_DEPENDS "test/*.cpp")
   cpp_test("${target_lib}" ${test_src})
endif ()


target_link_libraries("${target_lib}" PUBLIC
    "kae"
)
