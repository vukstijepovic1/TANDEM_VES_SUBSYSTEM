cmake_minimum_required(VERSION 3.20)

project (ubx VERSION 1.0.0)

set(SOURCES
	./ubx_common.c
	./ubx_parser.c
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
SYSTEM PUBLIC
	./
)

target_link_libraries(${PROJECT_NAME}
	${DEVICE}
)