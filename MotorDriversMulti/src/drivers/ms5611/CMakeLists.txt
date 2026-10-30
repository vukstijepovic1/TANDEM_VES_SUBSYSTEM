cmake_minimum_required(VERSION 3.20)

project (ms5611)

set(SOURCES
	ms5611.c
	ms5611_i2c.c
	ms5611_spi.c
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