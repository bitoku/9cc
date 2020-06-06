cmake_minimum_required(VERSION 3.16)
project(9cc C)

set(CMAKE_C_STANDARD 11)
#set(CMAKE_C_FLAGS "-static")

add_custom_target(test
        ../test.bash
)
add_executable(9cc main.c token.c token.h)