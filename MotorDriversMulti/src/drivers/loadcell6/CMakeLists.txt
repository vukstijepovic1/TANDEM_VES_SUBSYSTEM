cmake_minimum_required(VERSION 3.20)

project (loadcell6)

set(SOURCES
	loadcell6.c
)

add_library(${PROJECT_NAME}
STATIC
	${SOURCES}
)

target_include_directories(${PROJECT_NAME}
PUBLIC
	./
	${CMAKE_SOURCE_DIR}/devices/lpc1768/HAL
	${CMAKE_SOURCE_DIR}/devices/lpc1768/drivers
)

target_link_libraries(${PROJECT_NAME}
PUBLIC
	${DEVICE}
)