cmake_minimum_required(VERSION 3.13)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include("algue/add_clang_tests")
include("algue/cpp_compiler")
include("algue/cpp_flags")
include("algue/cpp_warnings")
include("algue/set_default")


if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(IS_ROOT_LIST ON)

    set_default(CMAKE_BUILD_TYPE "Debug" STRING)
    set_default(CMAKE_MESSAGE_CONTEXT_SHOW ON BOOL)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
else ()
    set(IS_ROOT_LIST OFF)
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(IS_DEBUG ON)
else ()
    set(IS_DEBUG OFF)
endif ()


option(ALGUE_TESTS "Build tests" ${IS_ROOT_LIST})
option(ALGUE_AUTO_COMPILERS "Automatically override compilers" ${IS_ROOT_LIST})
option(ALGUE_AUTO_FLAGS "Automatically override flags" ${IS_ROOT_LIST})
set(ALGUE_ARCH "native" CACHE STRING "Machine architecture")
set(ALGUE_SANITIZERS "address,undefined" CACHE STRING "Sanitizers to use (only in Debug)")
option(BUILD_SHARED_LIBS "Link shared libs instead of static libs" ${IS_DEBUG})


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
set(CMAKE_OPTIMIZE_DEPENDENCIES ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if (ALGUE_AUTO_FLAGS)
    cpp_flags_auto("${ALGUE_ARCH}" "${ALGUE_SANITIZERS}")
    cpp_warnings_auto()
endif ()

if (ALGUE_TESTS)
    enable_testing()
    if (IS_ROOT_LIST)
        add_clang_tests("${PROJECT_SOURCE_DIR}")
    endif ()
endif ()

file(GLOB_RECURSE data RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/data" CONFIGURE_DEPENDS "data/*")
foreach (file IN LISTS data)
    add_custom_command(OUTPUT "${CMAKE_BINARY_DIR}/data/${file}"
        COMMAND cmake -E copy "${CMAKE_CURRENT_SOURCE_DIR}/data/${file}" "${CMAKE_BINARY_DIR}/data/"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/data/${file}"
        COMMENT "Copying data/${file}..."
        VERBATIM
    )
    add_custom_target("data_${file}" ALL DEPENDS "${CMAKE_BINARY_DIR}/data/${file}")
endforeach ()


add_subdirectory("third_party/doctest")
add_subdirectory("third_party/fmt")
add_subdirectory("third_party/libressl")
add_subdirectory("third_party/sqlite")
add_subdirectory("third_party/asio")

set(KAE_TESTS ${ALGUE_TESTS})
add_subdirectory("kae")

add_subdirectory("http")
add_subdirectory("json")
add_subdirectory("utils")

add_subdirectory("algued")
add_subdirectory("lcu_api_explorer")
