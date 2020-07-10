cmake_minimum_required(VERSION 3.12)
include(FetchContent)

FetchContent_Declare("kae"
    GIT_REPOSITORY "https://github.com/kiwixz/kae"
    GIT_TAG "df11511ba77bdbc2f9892d4b61cbcc3ad3d3fc64"
)

FetchContent_GetProperties("kae")
if (kae_POPULATED)
    message(STATUS "third party 'kae' already populated, skipping")
    return()
endif()

FetchContent_Populate("kae")
add_subdirectory("${kae_SOURCE_DIR}" "${kae_BINARY_DIR}")
