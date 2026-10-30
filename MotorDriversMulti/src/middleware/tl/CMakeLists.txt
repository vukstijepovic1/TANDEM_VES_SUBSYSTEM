cmake_minimum_required(VERSION 3.20)

project (tl)

set(SOURCES
    tl_common.c
)

add_library(${PROJECT_NAME}
STATIC
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
PUBLIC
    ./
)