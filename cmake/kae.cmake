cmake_minimum_required(VERSION 3.12)
include(FetchContent)

FetchContent_Declare("kae"
    GIT_REPOSITORY "https://github.com/kiwixz/kae"
    GIT_TAG "dc0dfd547723fc4a26b9aaa4dbdde6f2461c8212"
)

FetchContent_GetProperties("kae")
if (kae_POPULATED)
    message(STATUS "third party 'kae' already populated, will not add this one")
    set(add_this_kae OFF)
else ()
    set(add_this_kae ON)
    message(STATUS "fetching kae")
    FetchContent_Populate("kae")
endif ()


list(APPEND CMAKE_MODULE_PATH "${kae_SOURCE_DIR}/cmake")


function (add_kae)
    set(files_to_import
        ".clang-format"
        ".clang-tidy"
        "format_all.py"
    )
    foreach (file ${files_to_import})
        file(CREATE_LINK "${kae_SOURCE_DIR}/${file}" "${PROJECT_SOURCE_DIR}/${file}" SYMBOLIC)
    endforeach ()

    if (add_this_kae)
        message(STATUS "adding kae")
        list(APPEND CMAKE_MESSAGE_CONTEXT "kae")
        add_subdirectory("${kae_SOURCE_DIR}" "${kae_BINARY_DIR}")
        list(POP_BACK CMAKE_MESSAGE_CONTEXT)
    endif ()
endfunction ()
