cmake_minimum_required(VERSION 3.20)

project (lfs)

add_library(littlefs INTERFACE)

target_compile_options(littlefs
INTERFACE 
    -Wno-shadow
)

target_sources(littlefs
INTERFACE
    ./littlefs/lfs.c
    ./littlefs/lfs_util.c
)

target_compile_definitions(littlefs INTERFACE
    LFS_DEFINES=lfs_defines.h
)

target_include_directories(littlefs
INTERFACE
    ./littlefs
    .
)

add_library(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
PUBLIC
    ./
)

target_link_libraries(${PROJECT_NAME}
    littlefs
)