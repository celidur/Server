cmake_minimum_required(VERSION 3.23)
project(server_ C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(server_ main.c server.c server.h)
