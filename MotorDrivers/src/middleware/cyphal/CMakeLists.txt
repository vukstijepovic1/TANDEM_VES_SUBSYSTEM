add_library(cyphal_utility STATIC
    cyphal_utility.c
    cyphal_recieve_handlers.c 
)

target_include_directories(cyphal_utility
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_SOURCE_DIR}/devices/lpc1768/HAL
    ${CMAKE_SOURCE_DIR}/src/middleware/FreeRTOS/Source/include
    ${CMAKE_SOURCE_DIR}/src/middleware/FreeRTOS/Source/portable/GCC/ARM_CM3
)

target_link_libraries(cyphal_utility
PUBLIC
    libcanard
)
