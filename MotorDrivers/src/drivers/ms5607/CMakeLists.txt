cmake_minimum_required(VERSION 3.20)

project (ms5607)

set(SOURCES
	ms5607.c
	ms5607_i2c.c
	ms5607_spi.c
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