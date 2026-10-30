cmake_minimum_required(VERSION 3.20)

project (xbus)

set(SOURCES
	xbus.c
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
SYSTEM PUBLIC
	./
)