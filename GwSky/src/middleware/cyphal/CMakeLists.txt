add_library(cyphal_utility STATIC
    cyphal_utility.c
)

target_include_directories(cyphal_utility
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_SOURCE_DIR}/devices/lpc1768/HAL
)

target_link_libraries(cyphal_utility
PUBLIC
    libcanard
)
