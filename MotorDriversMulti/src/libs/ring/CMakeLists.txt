cmake_minimum_required(VERSION 3.20)

project (ring)

set(SOURCES
    ring.c
)

add_library(${PROJECT_NAME}
STATIC
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
PUBLIC
	./
)