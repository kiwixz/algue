cmake_minimum_required(VERSION 3.12)
include(FetchContent)

FetchContent_Declare("kae"
    GIT_REPOSITORY "https://github.com/kiwixz/kae"
    GIT_TAG "ef749213f725db63f57bdf13704dbcd830aedd33"
)

FetchContent_GetProperties("kae")
if (kae_POPULATED)
    message(STATUS "third party 'kae' already populated, will not add this one")
else ()
    message(STATUS "fetching kae")
    FetchContent_Populate("kae")

    function (add_kae)
        message(STATUS "adding kae")
        list(APPEND CMAKE_MESSAGE_CONTEXT "kae")
        add_subdirectory("${kae_SOURCE_DIR}" "${kae_BINARY_DIR}")
        list(POP_BACK CMAKE_MESSAGE_CONTEXT)
    endfunction ()
endif ()


list(APPEND CMAKE_MODULE_PATH "${kae_SOURCE_DIR}/cmake")
