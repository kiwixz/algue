cmake_minimum_required(VERSION 3.12)
include(FetchContent)

FetchContent_Declare("kae"
    GIT_REPOSITORY "https://github.com/kiwixz/kae"
    GIT_TAG "55fde9b256cbdc423edd788284644ad96bb7569c"
)

FetchContent_GetProperties("kae")
if (kae_POPULATED)
    message(STATUS "third party 'kae' already populated, skipping")
    return()
endif()

FetchContent_Populate("kae")
add_subdirectory("${kae_SOURCE_DIR}" "${kae_BINARY_DIR}")

list(APPEND CMAKE_MODULE_PATH "${kae_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" PARENT_SCOPE)
