set(kae_cmake_dir "${CMAKE_CURRENT_LIST_DIR}")

function (cpp_test target)
    if (NOT ARGN)
        return ()
    endif ()

    set(test_target "${target}_test")

    add_executable("${test_target}" ${ARGN})
    target_link_libraries("${test_target}" PRIVATE "${target}" "doctest_with_main")

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options("${test_target}" PRIVATE "-fno-sanitize-recover=all")
    endif ()

    add_test(NAME "${test_target}" COMMAND "${test_target}")
endfunction ()
