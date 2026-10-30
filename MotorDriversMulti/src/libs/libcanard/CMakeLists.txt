cmake_minimum_required(VERSION 3.20)

project (libcanard)

set(SOURCES
	canard.c
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
SYSTEM PUBLIC
	./
)

target_link_libraries(${PROJECT_NAME}
	${DEVICE}
)