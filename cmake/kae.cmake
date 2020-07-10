cmake_minimum_required(VERSION 3.12)
include(FetchContent)

FetchContent_Declare("kae"
    GIT_REPOSITORY "https://github.com/kiwixz/kae"
    GIT_TAG "8d446b3d4c5b964406b99abdbea1d06157868a45"
)

FetchContent_GetProperties("kae")
if (kae_POPULATED)
    message(STATUS "third party 'kae' already populated, will not add this one")
    set(add_this_kae OFF)
else ()
    set(add_this_kae ON)
    FetchContent_Populate("kae")
endif ()


list(APPEND CMAKE_MODULE_PATH "${kae_SOURCE_DIR}/cmake")

set(kae_root "${CMAKE_CURRENT_LIST_DIR}/..")
file(CREATE_LINK "${kae_SOURCE_DIR}/.clang-format" "${kae_root}/.clang-format" SYMBOLIC)
file(CREATE_LINK "${kae_SOURCE_DIR}/.clang-tidy" "${kae_root}/.clang-tidy" SYMBOLIC)


function (add_kae)
    if (add_this_kae)
        add_subdirectory("${kae_SOURCE_DIR}" "${kae_BINARY_DIR}")
    endif ()
endfunction ()
