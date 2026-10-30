cmake_minimum_required(VERSION 3.20)

project(vesc)

set(SOURCES
    ./vesc.c
    ./vesc.h
    ./vesc_helper.c
    ./vesc_helper.h
    ./vesc_mw.c
    ./vesc_mw.h
)

add_library(${PROJECT_NAME}
    STATIC
    ${SOURCES}
)

set(DRIVERS
    ${LSM_SOURCE_DIR}/src/drivers/vesc
)

set(DEVICE_LPC1768
    ${LSM_SOURCE_DIR}/devices/lpc1768/HAL
    ${LSM_SOURCE_DIR}/devices/lpc1768/core
)

set(MIDDLEWARE
    ${LSM_SOURCE_DIR}/src/middleware/lfs/littlefs
    ${LSM_SOURCE_DIR}/src/middleware/mav
    ${LSM_SOURCE_DIR}/src/middleware/tl
    ${LSM_SOURCE_DIR}/src/middleware/FreeRTOS/include
    ${LSM_SOURCE_DIR}/src/middleware/FreeRTOS/portable/GCC/ARM_CM3
)

set(OTHER
    ${LSM_BINARY_DIR}/mavlink
    ${LSM_SOURCE_DIR}/src/app
)

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC
    ${DRIVERS}
    ${DEVICE_LPC1768}
    ${MIDDLEWARE}
    ${OTHER}
)