cmake_minimum_required(VERSION 3.12)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include("kae")

include("kae/add_clang_tests")
include("kae/cpp_compiler")
include("kae/cpp_flags")
include("kae/cpp_warnings")
include("kae/set_default")


if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(IS_ROOT_LIST ON)
else ()
    set(IS_ROOT_LIST OFF)
endif ()


option(ALGUE_TESTS "Build tests" ${IS_ROOT_LIST})
option(ALGUE_AUTO_COMPILERS "Automatically override compilers" ${IS_ROOT_LIST})
option(ALGUE_AUTO_FLAGS "Automatically override flags" ${IS_ROOT_LIST})

if (IS_ROOT_LIST)
    set_default(CMAKE_BUILD_TYPE "Debug" STRING)
    set_default(CMAKE_MESSAGE_CONTEXT_SHOW ON BOOL)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif ()


if (ALGUE_AUTO_COMPILERS)
    cpp_compiler_auto()
endif ()

project("algue"
    VERSION 0.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (ALGUE_AUTO_FLAGS)
    cpp_flags_auto()
    cpp_warnings_auto()
endif ()

if (ALGUE_TESTS)
    enable_testing()
    if (IS_ROOT_LIST)
        add_clang_tests("${PROJECT_SOURCE_DIR}")
    endif ()
endif ()


add_kae()
add_subdirectory("third_party/libressl")
add_subdirectory("third_party/asio")

add_subdirectory("http")
add_subdirectory("json")
add_subdirectory("utils")
add_subdirectory("lcu_api_explorer")
