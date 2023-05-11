cmake_minimum_required(VERSION 3.25)
project(opertaion_systems C)

set(CMAKE_C_STANDARD 11)

include_directories(Task_2)

add_executable(opertaion_systems
        Ex1/keyGenerator.c

        Task_2/Part_a/cmp.c
        Task_2/Part_a/copy.c
        Task_2/Part_b/codecA.c
        Task_2/Part_b/codecB.c
        Task_2/Part_b/encode.c
        Task_2/Part_b/decode.c Task_2/Part_c/stshell.c Task_2/Part_c/stshell.c)
