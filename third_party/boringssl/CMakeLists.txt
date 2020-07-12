cmake_minimum_required(VERSION 3.12)
include("kae/third_party")

if (WIN32)
    third_party("perl" URL "http://strawberryperl.com/download/5.30.2.1/strawberry-perl-5.30.2.1-64bit-portable.zip" DONT_RUN_CMAKE)
    FetchContent_GetProperties("perl")
    set(PERL_EXECUTABLE "${perl_SOURCE_DIR}/perl/bin/perl.exe")

    third_party("go" URL "https://golang.org/dl/go1.14.4.windows-amd64.zip" DONT_RUN_CMAKE)
    FetchContent_GetProperties("go")
    set(GO_EXECUTABLE "${go_SOURCE_DIR}/bin/go.exe")

    third_party("nasm" URL "https://www.nasm.us/pub/nasm/releasebuilds/2.15.02/win64/nasm-2.15.02-win64.zip" DONT_RUN_CMAKE)
    FetchContent_GetProperties("nasm")
    set(CMAKE_ASM_NASM_COMPILER "${nasm_SOURCE_DIR}/nasm.exe")
endif ()

set(BORINGSSL_ALLOW_CXX_RUNTIME ON)
third_party("boringssl"
    REPO "google/boringssl"
    TAG "8f88b27d6add3fc8f9ee548050e4958efa239f1f"
    PATCHES "000-keep_flags.patch"
)
FetchContent_GetProperties("boringssl")

add_library("boringssl" INTERFACE)
target_include_directories("boringssl" SYSTEM INTERFACE "${boringssl_SOURCE_DIR}/include")
target_link_libraries("boringssl" INTERFACE "ssl")
