cmake_minimum_required(VERSION 3.20)

project (mav)

set(MAVLINK_DIR "${CMAKE_CURRENT_LIST_DIR}/mavlink")
set(MAVLINK_BINARY_DIR "${CMAKE_BINARY_DIR}/mavlink")
set(MAVLINK_DIALECT "lisum")

if (NOT EXISTS ${LSM_MESSAGES_DIR}/mavlink/${MAVLINK_DIALECT}.xml)
    message(FATAL_ERROR "mavgen: MAVLink dialect not found!")
endif()

execute_process(
    COMMAND ${PYTHON_EXECUTABLE} ${MAVLINK_DIR}/pymavlink/tools/mavgen.py
    --lang C --wire-protocol 2.0
    --output ${MAVLINK_BINARY_DIR}
    ${LSM_MESSAGES_DIR}/mavlink/${MAVLINK_DIALECT}.xml
)

add_library(mavlink_generated INTERFACE)
target_compile_options(mavlink_generated
INTERFACE 
    -Wno-address-of-packed-member
    -Wno-cast-align
    -Wno-shadow
    -Wno-stack-usage
    -Wno-conversion
)
target_sources(mavlink_generated
INTERFACE
    ${MAVLINK_BINARY_DIR}/${MAVLINK_DIALECT}/${MAVLINK_DIALECT}.h
)

set(SOURCES
    mav.c
    mavlink_wrapper.c
)

add_library(${PROJECT_NAME}
STATIC
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
PUBLIC
    ${MAVLINK_BINARY_DIR}
    ${MAVLINK_BINARY_DIR}/${MAVLINK_DIALECT}
    ./
)

target_link_libraries(${PROJECT_NAME} tl mavlink_generated)