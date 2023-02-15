cmake_minimum_required(VERSION 3.24)
project(Server C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(
        Bytes.c
        Bytes.h
        rsa.c
        rsa.h)
