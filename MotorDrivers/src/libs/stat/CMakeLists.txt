cmake_minimum_required(VERSION 3.20)

project (stat)

set(SOURCES
	stat.c
)

add_library(${PROJECT_NAME}
STATIC
	${SOURCES}
)

target_include_directories(${PROJECT_NAME}
PUBLIC
	./
)

target_link_libraries(${PROJECT_NAME} m)