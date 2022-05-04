cmake_minimum_required(VERSION 3.10.2)
project(C_Socket C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS -pthread)

include_directories(.)

add_executable(
        Server
        src/main.c
        src/keyValueStore.c include/keyValueStore.h
        src/configuration.c include/configuration.h
        src/subroutines.c include/subroutines.h
        src/server.c include/server.h
)

add_executable(
        Client
        src/client.c include/client.h
        src/keyValueStore.c include/keyValueStore.h
        src/configuration.c include/configuration.h
        src/subroutines.c include/subroutines.h
)

add_executable(
        Test
        include/minunit.h
        test/subroutinesTest.c
        src/keyValueStore.c include/keyValueStore.h
        src/configuration.c include/configuration.h
        src/subroutines.c include/subroutines.h
)

add_test(Test Test)