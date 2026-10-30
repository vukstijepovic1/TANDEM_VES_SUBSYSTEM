cmake_minimum_required(VERSION 3.20)

project (icm42688p)

set(SOURCES
    icm42688p_spi.c
    icm42688p.c
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME}
SYSTEM PUBLIC
	./
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
  	${DEVICE}
)