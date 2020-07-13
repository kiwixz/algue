cmake_minimum_required(VERSION 3.12)

set(target "lcu_api")

add_executable("${target}" "src/main.cpp")
target_link_libraries("${target}" PUBLIC
    "kae::kae"
    "asio::asio"

    "http"
    "utils"
)
