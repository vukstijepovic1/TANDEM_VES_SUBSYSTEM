cmake_minimum_required(VERSION 3.20)

project (ak09915c)

set(SOURCES
    ak09915c.c
    ak09915c_i2c.c
    ak09915c_spi.c
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