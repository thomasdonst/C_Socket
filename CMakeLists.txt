cmake_minimum_required(VERSION 3.10.2)
project(C_Socket C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(C_Socket
        main.c
        main.h)
