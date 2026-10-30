cmake_minimum_required(VERSION 3.20)

project (ulog)

set(ULOG_GEN_DIR "${CMAKE_CURRENT_LIST_DIR}/ulog-gen")
set(ULOG_BINARY_DIR "${CMAKE_BINARY_DIR}/ulog")
set(ULOG_XML "lisum")

if (NOT EXISTS ${LSM_MESSAGES_DIR}/mavlink/${ULOG_XML}.xml)
    message(FATAL_ERROR "ulog-gen: XML file not found")
endif()

execute_process(
    COMMAND ${PYTHON_EXECUTABLE} ${ULOG_GEN_DIR}/ulog_gen.py
    -o ${ULOG_BINARY_DIR}
    ${LSM_MESSAGES_DIR}/mavlink/${ULOG_XML}.xml
)

file(GLOB SOURCES CONFIGURE_DEPENDS ${ULOG_BINARY_DIR}/*.c)
list(APPEND SOURCES ulog.c)

add_library(${PROJECT_NAME}
STATIC 
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
SYSTEM PUBLIC
    ${ULOG_BINARY_DIR}
    ./
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    lfs
)