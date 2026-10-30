cmake_minimum_required(VERSION 3.20)

project (udp)

set(SOURCES
    udp_crc.c
    udp.c
)

add_library(${PROJECT_NAME}
STATIC
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
PUBLIC
    ./
)