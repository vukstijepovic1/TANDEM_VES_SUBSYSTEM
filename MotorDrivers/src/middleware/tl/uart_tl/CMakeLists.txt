cmake_minimum_required(VERSION 3.20)

project (uart_tl)

set(SOURCES
    uart_tl.c
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
	${DEVICE}
    tl
	freertos_kernel
)