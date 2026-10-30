cmake_minimum_required(VERSION 3.20)

project (epos)

set(SOURCES
	epos_common.c
	epos_csp.c
	epos_eeprom.c
	epos_homing.c
	epos_od.c
	epos_ppm.c
	epos.c
)

add_library(${PROJECT_NAME}
STATIC
	${SOURCES}
)

target_include_directories(${PROJECT_NAME}
SYSTEM PUBLIC
	./
)

target_link_libraries(${PROJECT_NAME}
	canopen
)