cmake_minimum_required(VERSION 3.16)
project(9cc C)

set(CMAKE_C_STANDARD 11)

add_executable(9cc main.c token.c parse.c codegen.c)
add_executable(test test.c)
add_dependencies(test 9cc)