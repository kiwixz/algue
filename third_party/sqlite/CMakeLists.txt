cmake_minimum_required(VERSION 3.12)
include("algue/third_party")

set(version "3330000")  # 3.33.0
third_party("sqlite" URL "https://www.sqlite.org/2020/sqlite-amalgamation-${version}.zip" DONT_RUN_CMAKE)
suppress_warnings_cpp()

project("sqlite" C)

FetchContent_GetProperties("sqlite")
add_library("sqlite" EXCLUDE_FROM_ALL "${sqlite_SOURCE_DIR}/sqlite3.c")
add_library("sqlite::sqlite" ALIAS "sqlite")

target_compile_definitions("sqlite" PRIVATE
    "SQLITE_DQS=0"
    "SQLITE_DEFAULT_MEMSTATUS=0"
    "SQLITE_DEFAULT_WAL_SYNCHRONOUS=1"
    "SQLITE_LIKE_DOESNT_MATCH_BLOBS"
    "SQLITE_MAX_EXPR_DEPTH=0"
    "SQLITE_OMIT_DECLTYPE"
    "SQLITE_OMIT_DEPRECATED"
    "SQLITE_OMIT_PROGRESS_CALLBACK"
    "SQLITE_OMIT_SHARED_CACHE"
    "SQLITE_USE_ALLOCA"
    "SQLITE_OMIT_AUTOINIT"

    "__PRFCHWINTRIN_H"  # clang march=native workaround (https://lists.llvm.org/pipermail/llvm-dev/2020-June/142227.html)
)
target_include_directories("sqlite" SYSTEM PUBLIC "${sqlite_SOURCE_DIR}")
