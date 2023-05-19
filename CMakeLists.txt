cmake_minimum_required(VERSION 3.18)
project(Server C)

set(CMAKE_C_STANDARD 11)

set(GMP_DIR "/opt/homebrew/Cellar/gmp/6.2.1_1")


include_directories(.)

add_executable(Server
        rsa.c
        main.c server.c
        Buffer.c)

include_directories(${PROJECT_NAME} ${GMP_DIR}/include)

target_link_libraries(${PROJECT_NAME}
        "${GMP_DIR}/lib/libgmp.a"
        "${GMP_DIR}/lib/libgmpxx.a"
        )
