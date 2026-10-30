cmake_minimum_required(VERSION 3.20)

project (canopen)

set(SOURCES
	canopen.c
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
SYSTEM PUBLIC
	./
)

target_link_libraries(${PROJECT_NAME}
	${DEVICE}
)