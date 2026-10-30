cmake_minimum_required(VERSION 3.20)

project (udp_tl)

set(SOURCES
    udp_tl.c
)

add_library(${PROJECT_NAME}
STATIC
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
PUBLIC
	./
)

target_link_libraries(${PROJECT_NAME}
    udp
    tl
)