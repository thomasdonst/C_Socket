cmake_minimum_required(VERSION 3.10.2)
project(C_Socket C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(
        C_Socket
        src/main.c include/main.h
        src/client.c include/client.h src/server.c include/server.h src/configuration.c include/configuration.h src/subroutines.c include/subroutines.h src/keyValueStore.c include/keyValueStore.h)
