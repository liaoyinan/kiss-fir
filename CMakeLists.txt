cmake_minimum_required(VERSION 3.17)
project(kiss-fir C)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_executable(kiss_fir_test
        kiss-fir.c
        kiss-fir.h
        test.c)
