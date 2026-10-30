cmake_minimum_required(VERSION 3.20)

project (anpp)

set(SOURCES
	anpp_common.c
	anpp.c
)

add_library(${PROJECT_NAME}
STATIC
	${SOURCES}
)

target_include_directories(${PROJECT_NAME}
SYSTEM PUBLIC
	./
)