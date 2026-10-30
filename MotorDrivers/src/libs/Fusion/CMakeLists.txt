cmake_minimum_required(VERSION 3.20)

project (Fusion)

set(SOURCES
	Fusion/Fusion/FusionAhrs.c
    Fusion/Fusion/FusionCompass.c
    Fusion/Fusion/FusionOffset.c
)

add_library(${PROJECT_NAME}
STATIC
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
SYSTEM PUBLIC
    ./Fusion/Fusion/
)

target_link_libraries(${PROJECT_NAME} m)