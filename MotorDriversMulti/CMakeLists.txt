cmake_minimum_required(VERSION 3.20)

project(app_shell
    VERSION 1.0.0
    DESCRIPTION "Motor Driver project."
)

enable_language(
    C
    CXX
    ASM
)

# Optional: enable raw Cyphal heartbeat over UDP for testing
option(CYPHAL_UDP_TEST "Send raw serialized Cyphal Heartbeat over UDP" OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

#
# === Build Metadata Extraction ===
#

# Build timestamp (UNIX epoch)
string(TIMESTAMP BUILD_UNIX_TIMESTAMP "%s" UTC)

# Git commit short hash (8 chars)
execute_process(
    COMMAND git rev-parse --short=8 HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Latest Git tag (e.g. "v0.1.0")
execute_process(
    COMMAND git describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT GIT_TAG)
    set(GIT_TAG "v0.0.0")
endif()

# FreeRTOS version (read from manifest.yml)
file(READ ${CMAKE_SOURCE_DIR}/src/middleware/FreeRTOS/manifest.yml FREERTOS_MANIFEST)
string(REGEX MATCH "version:[ \t]*\"([^\"]+)\"" _match "${FREERTOS_MANIFEST}")
set(OS_VERSION "${CMAKE_MATCH_1}")
if(NOT OS_VERSION)
    set(OS_VERSION "v0.0.0")
endif()

# LittleFS version (read from lfs.h)
file(READ ${CMAKE_SOURCE_DIR}/src/middleware/lfs/littlefs/lfs.h LFS_HEADER)
string(REGEX MATCH "#define LFS_VERSION[ \t]+0x[0-9a-fA-F]+" LFS_VERSION_LINE "${LFS_HEADER}")
string(REGEX REPLACE ".*0x" "" LFS_VERSION_BUILD "${LFS_VERSION_LINE}")

#
# === Logging (optional) ===
#
message(STATUS "Git Tag           : ${GIT_TAG}")
message(STATUS "Git Commit Hash   : ${GIT_HASH}")
message(STATUS "Build Timestamp   : ${BUILD_UNIX_TIMESTAMP}")
message(STATUS "FreeRTOS OS_VERSION : ${OS_VERSION}")
message(STATUS "LittleFS Version  : ${LFS_VERSION_BUILD}")

#
# === Configure header with build metadata ===
#
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config/main.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/main.h
    @ONLY
)


set(LSM_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE FILEPATH "Project source directory" FORCE)
set(LSM_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE FILEPATH "Project binary directory" FORCE)

if (EXISTS ${LSM_SOURCE_DIR}/.venv)
    set(PYTHON_VENV_PATH "${LSM_SOURCE_DIR}/.venv")
elseif (EXISTS ${LSM_SOURCE_DIR}/venv)
    set(PYTHON_VENV_PATH "${LSM_SOURCE_DIR}/venv")
elseif (EXISTS ${LSM_SOURCE_DIR}/python_venv)
    set(PYTHON_VENV_PATH "${LSM_SOURCE_DIR}/python_venv")
else ()
    message(FATAL_ERROR "Python Virtual Environment not found!")
endif ()

if (CMAKE_HOST_WIN32)
    string(APPEND PYTHON_VENV_PATH "/Scripts")
elseif (CMAKE_HOST_UNIX)
    string(APPEND PYTHON_VENV_PATH "/bin")
endif ()

string(APPEND PYTHON_VENV_PATH "/python")

set(PYTHON_EXECUTABLE ${PYTHON_VENV_PATH} CACHE FILEPATH "Project python executable" FORCE)

# Messages Directory
if (EXISTS ${LSM_SOURCE_DIR}/messages)
    set(LSM_MESSAGES_DIR "${LSM_SOURCE_DIR}/messages" CACHE FILEPATH "Messages directory" FORCE)
else ()
    message(FATAL_ERROR "Messages folder not found!")
endif ()

set(DEVICE lpc1768)

# ==============================================================================
#	Add Common Flags
# ==============================================================================

# =============================================================
#	MCU flags
# =============================================================
set(MCU_FLAGS
    -mcpu=cortex-m3
    -mthumb
)

set(FREERTOS_FLAGS
    -fno-strict-aliasing
    -fno-common
    -fno-inline
    -fno-tree-loop-distribute-patterns
    -fsigned-char
    -fno-builtin
)

# =============================================================
# Config Extra options
# (https://gcc.gnu.org/onlinedocs/gcc/Code-Gen-Options.html)
# (https://gcc.gnu.org/onlinedocs/gcc-2.95.2/gcc_2.html#SEC10)
# =============================================================
set(EXTRA_OPTIONS
	-fdata-sections			# Place each function or data item into its own
							# section in the output file if the target supports
							# arbitrary sections.

	-ffunction-sections		# Place each function or data item into its own
							# section in the output file if the target supports
							# arbitrary sections.

	-fno-common				# Allocate even uninitialized global variables in
							# the bss section of the object file, rather than
							# generating them as common blocks.

	-fno-tree-loop-if-convert	#

	-fno-merge-constants	# do not attempt to merge identical constants
							# (string constants and floating-point constants)
							# across compilation units.

	-fshort-enums			# Allocate to an enum type only as many bytes as it
							# needs for the declared range of possible values.

	-fno-math-errno			# Do not set ERRNO after calling math functions that
							# are executed with a single instruction, e.g., 
                            # sqrt. A program that relies on IEEE exceptions for
                            # math error handling may want to use this flag for
                            # speed while maintaining IEEE arithmetic
                            # compatibility.
)

# =============================================================
#	Config debug info
# =============================================================
set(DEBUG_INFO
    -g3
    -gdwarf-2
)

# =============================================================
# Config dependancy
# =============================================================
set(DEPENDENCY_INFO
    -MMD
    -MP
    -MF "$(@:%.0=%.d)"
)

# =============================================================
#	Configure optimization levels
# =============================================================
set(OPT
    $<$<CONFIG:Debug>:-Og>
    $<$<CONFIG:Release>:-O3>
)

# =============================================================
#	Add compile options
# =============================================================
add_compile_options(
    ${MCU_FLAGS}
    ${EXTRA_OPTIONS}
    ${DEBUG_INFO}
    ${DEPENDENCY_INFO}
    ${OPT}
    ${PACKED_MEMBER_WARNING}
    ${FREERTOS_FLAGS}
)

# ==============================================================================
#	Device configuration
# ==============================================================================
if (EXISTS ${LSM_SOURCE_DIR}/config)
    include_directories(config)
endif ()

if(EXISTS ${LSM_SOURCE_DIR}/config/dev_config.h)
    add_compile_definitions(DEV_CONFIG)
endif()

if(EXISTS ${LSM_SOURCE_DIR}/config/mav_config.h)
    add_compile_definitions(MAV_CONFIG)
endif()

if(EXISTS ${LSM_SOURCE_DIR}/config/flashconf.h)
    add_compile_definitions(FLASH_CONFIG)
endif()

if(EXISTS ${LSM_SOURCE_DIR}/src/drivers/lfs_utilities/lfs_def.h)
    add_compile_definitions(LFS_UTILITIES_CONFIG)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/src)
    add_compile_definitions(SOURCE)
endif()

# ==============================================================================
#	GLOBAL INCLUDE DIRECTORIES - DODAJTE OVDE
# ==============================================================================
include_directories(
    ${CMAKE_SOURCE_DIR}/src/app
    ${CMAKE_BINARY_DIR}/cyphal
    ${CMAKE_BINARY_DIR}/cyphal/cyphal
    ${CMAKE_BINARY_DIR}/cyphal/uavcan
    ${CMAKE_BINARY_DIR}/cyphal/reg
    ${CMAKE_BINARY_DIR}/cyphal/mavlink
    ${CMAKE_BINARY_DIR}/cyphal/utility

    ${CMAKE_SOURCE_DIR}/src/libs/libcanard
    ${CMAKE_SOURCE_DIR}/src/libs/libcanard/src

    ${CMAKE_SOURCE_DIR}/src/middleware/cyphal

    ${CMAKE_SOURCE_DIR}/src/middleware/FreeRTOS/Source/include
    ${CMAKE_SOURCE_DIR}/src/middleware/FreeRTOS/Source/portable/GCC/ARM_CM3
)

# Also add the Nunavut support headers if found
if (EXISTS "${NUNAVUT_SUPPORT_DIR}")
    include_directories("${NUNAVUT_SUPPORT_DIR}")
endif()

if(EXISTS ${LSM_SOURCE_DIR}/config/dev_config.h)
    add_compile_definitions(DEV_CONFIG)
endif()

# ==============================================================================
#	Add Subdirectories
# ==============================================================================

# =============================================================
#	devices
# =============================================================
add_subdirectory(devices/${DEVICE} ${LSM_BINARY_DIR}/devices/${DEVICE})

# =============================================================
#	drivers
# =============================================================
add_subdirectory(src/drivers/ak09915c ${LSM_BINARY_DIR}/src/drivers/ak09915c)
add_subdirectory(src/drivers/anpp ${LSM_BINARY_DIR}/src/drivers/anpp)
add_subdirectory(src/drivers/bms ${LSM_BINARY_DIR}/src/drivers/bms)
add_subdirectory(src/drivers/epos ${LSM_BINARY_DIR}/src/drivers/epos)
add_subdirectory(src/drivers/icm42688p ${LSM_BINARY_DIR}/src/drivers/icm42688p)
add_subdirectory(src/drivers/motor ${LSM_BINARY_DIR}/src/drivers/motor)
add_subdirectory(src/drivers/loadcell6 ${LSM_BINARY_DIR}/src/drivers/loadcell6)
add_subdirectory(src/drivers/vesc ${LSM_BINARY_DIR}/src/drivers/vesc)
add_subdirectory(src/drivers/ms5607 ${LSM_BINARY_DIR}/src/drivers/ms5607)
add_subdirectory(src/drivers/ms5611 ${LSM_BINARY_DIR}/src/drivers/ms5611)
add_subdirectory(src/drivers/sd ${LSM_BINARY_DIR}/src/drivers/sd)
add_subdirectory(src/drivers/ubx ${LSM_BINARY_DIR}/src/drivers/ubx)


# =============================================================
#	libs
# =============================================================
add_subdirectory(src/libs/libcanard ${LSM_BINARY_DIR}/src/libs/libcanard)
add_subdirectory(src/libs/canopen ${LSM_BINARY_DIR}/src/libs/canopen)
add_subdirectory(src/libs/Fusion ${LSM_BINARY_DIR}/src/libs/Fusion)
add_subdirectory(src/libs/ring ${LSM_BINARY_DIR}/src/libs/ring)
add_subdirectory(src/libs/sbus ${LSM_BINARY_DIR}/src/libs/sbus)
add_subdirectory(src/libs/stat ${LSM_BINARY_DIR}/src/libs/stat)
add_subdirectory(src/libs/xbus ${LSM_BINARY_DIR}/src/libs/xbus)

# =============================================================
#	middleware
# =============================================================
set(FREERTOS_CONFIG_FILE_DIRECTORY ${LSM_SOURCE_DIR}/config CACHE STRING "Absolute path to the directory with FreeRTOSConfig.h")
set(FREERTOS_HEAP "4" CACHE STRING "FreeRTOS heap model number. 1 .. 5. Or absolute path to custom heap source file")
set(FREERTOS_PORT "GCC_ARM_CM3" CACHE STRING "FreeRTOS port name")

add_subdirectory(src/middleware/FreeRTOS ${LSM_BINARY_DIR}/src/middleware/FreeRTOS)
add_subdirectory(src/middleware/lfs ${LSM_BINARY_DIR}/src/middleware/lfs)
add_subdirectory(src/middleware/mav ${LSM_BINARY_DIR}/src/middleware/mav)
add_subdirectory(src/middleware/tl ${LSM_BINARY_DIR}/src/middleware/tl)
add_subdirectory(src/middleware/tl/uart_tl ${LSM_BINARY_DIR}/src/middleware/tl/uart_tl)
add_subdirectory(src/middleware/tl/udp_tl ${LSM_BINARY_DIR}/src/middleware/tl/udp_tl)
add_subdirectory(src/middleware/udp ${LSM_BINARY_DIR}/src/middleware/udp)
add_subdirectory(src/middleware/ulog ${LSM_BINARY_DIR}/src/middleware/ulog)
add_subdirectory(src/middleware/cyphal ${LSM_BINARY_DIR}/src/middleware/cyphal)

# ==============================================================================
#	Add Subdirectories
# ==============================================================================
set(PROJECT_SOURCES
    devices/${DEVICE}/startup.S
    src/app/main.c
    src/app/task_work.c
    src/app/task_bridge.c
    src/app/task_mav.c
    src/app/task_tx_can.c
)

set(EXECUTABLE program.elf)
add_executable(${EXECUTABLE} ${PROJECT_SOURCES})

target_include_directories(${EXECUTABLE}
PUBLIC
    src/app
    ${CMAKE_SOURCE_DIR}/generated_dsdl
    ${CMAKE_SOURCE_DIR}/devices/lpc1768/HAL
    ${CMAKE_SOURCE_DIR}/src/middleware/FreeRTOS/Source/include
    ${CMAKE_SOURCE_DIR}/src/middleware/FreeRTOS/Source/portable/GCC/ARM_CM3
    ${CMAKE_SOURCE_DIR}/src/middleware/lfs
    # Nunavut C headers root and direct access to namespace-only includes
    ${CMAKE_BINARY_DIR}/cyphal
    ${CMAKE_BINARY_DIR}/cyphal/cyphal
)

# Also include per-namespace Nunavut output roots for direct includes like <uavcan/node/Heartbeat_1_0.h>
target_include_directories(${EXECUTABLE} PRIVATE
    ${CMAKE_BINARY_DIR}/cyphal/uavcan
    ${CMAKE_BINARY_DIR}/cyphal/reg
    ${CMAKE_BINARY_DIR}/cyphal/mavlink
)

# Locate Nunavut C support headers (nunavut/support/serialization.h) via Python site-packages
# Ensure we have a Python interpreter available
if (NOT DEFINED PYTHON_EXECUTABLE)
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if (Python3_Interpreter_FOUND)
        set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
    else()
        set(PYTHON_EXECUTABLE python)
    endif()
endif()

execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "import os, nunavut, sys; sys.stdout.write(os.path.join(os.path.dirname(nunavut.__file__), 'support'))"
    OUTPUT_VARIABLE NUNAVUT_SUPPORT_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if (EXISTS "${NUNAVUT_SUPPORT_DIR}")
    message(STATUS "Nunavut support headers: ${NUNAVUT_SUPPORT_DIR}")
    target_include_directories(${EXECUTABLE} PRIVATE "${NUNAVUT_SUPPORT_DIR}")
endif()

target_compile_options(${DEVICE}
PRIVATE
	-w
    ${WARNING_OPTIONS}
)

# ==============================================================================
#	Add Linker Flags
# ==============================================================================
set(LINKER_FLAGS
    -specs=nosys.specs
    -lc
    -lm
    -lnosys
    -Wl,-Map=project.map,--cref
    -Wl,--gc-sections
    -Wl,--print-memory-usage
    -flto
    -fuse-linker-plugin
)

target_link_options(${EXECUTABLE}
PRIVATE
    -T${LSM_SOURCE_DIR}/devices/${DEVICE}/linker.ld
    ${MCU_FLAGS}
    ${LINKER_FLAGS}
)

set(WARNING_OPTIONS
	-Wall					# Enable all warnings.

	-Wextra					# Enable extra warning flash which are not enabled
                            # by -Wall.

	-Wstack-usage=128		# Warn if the stack usage of a function might exceed
							# ceratin value.

	-Wshadow				# Warn whenever a local variable or type declaration
							# shadowsnother variable, parameter, type,
							# class member (in C++),
							# or instance variable (in Objective-C) or whenever
                            # a built-in function is shadowed.

	-Wconversion			# Warn for implicit conversions that may alter a
                            # value.

	-Wdouble-promotion		# Give a warning when a value of type float is
							# implicitly promoted to double.

    -Wno-unused-parameter	# surpress "unused" variable warnings
    -Wno-unused-function	# surpress "unused" function warnings
    -Wno-enum-conversion	# surpress enum conversion warning
    -Wno-address-of-packed-member  # Suppress warnings when taking the address of a member inside a packed struct, which may cause unaligned pointer issues.

)

target_compile_options(${EXECUTABLE}
PRIVATE
    ${WARNING_OPTIONS}
)

# ==============================================================================
#	Link Libraries
# ==============================================================================

target_link_libraries(${EXECUTABLE}
    # device
    ${DEVICE}

    # drivers
    ak09915c
    anpp
    bms
    epos
    icm42688p
    motor
    loadcell6
    vesc
    ms5607
    ms5611
    sd
    ubx

    # libs
    libcanard
    canopen
    Fusion
    ring
    sbus
    stat
    xbus

    # middleware
    freertos_kernel
    mav
    tl
    uart_tl
    udp_tl
    lfs
    udp
    ulog
    cyphal_utility
)


# ----------------------------------------------------------------------------
# MAVLink -> DSDL conversion + Nunavut code generation for Cyphal (C headers)
# ----------------------------------------------------------------------------

set(MAVLINK_EXISTING_DSDL_ROOT "${CMAKE_BINARY_DIR}/mavlink_dsdl/mavlink")

# Proveri da li postoji 'common' ili 'lisum'
if(EXISTS "${MAVLINK_EXISTING_DSDL_ROOT}/common" OR EXISTS "${MAVLINK_EXISTING_DSDL_ROOT}/lisum")
    set(MAVLINK_DSDL_DIR "${MAVLINK_EXISTING_DSDL_ROOT}")
    set(USE_EXISTING_MAVLINK_DSDL TRUE)
else()
    set(MAVLINK_DSDL_DIR "${CMAKE_BINARY_DIR}/cyphal/cyphal/messages/mavlink")
    set(USE_EXISTING_MAVLINK_DSDL FALSE)
endif()

file(GLOB MAVLINK_MESSAGE_HEADERS
    "${CMAKE_BINARY_DIR}/mavlink/common/mavlink_msg_*.h"
    "${CMAKE_BINARY_DIR}/mavlink/lisum/mavlink_msg_*.h"
)

add_custom_command(
    OUTPUT "${MAVLINK_DSDL_DIR}/.mav2dsdl.stamp"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${MAVLINK_DSDL_DIR}"
    COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/src/libs/libcanard/mavlink_to_dsdl.py" 
        --mavlink-dirs "${CMAKE_BINARY_DIR}/mavlink/common" "${CMAKE_BINARY_DIR}/mavlink/lisum"
        --out-dir "${MAVLINK_DSDL_DIR}"
    COMMAND ${CMAKE_COMMAND} -E touch "${MAVLINK_DSDL_DIR}/.mav2dsdl.stamp"
    DEPENDS ${MAVLINK_MESSAGE_HEADERS} "${CMAKE_SOURCE_DIR}/src/libs/libcanard/mavlink_to_dsdl.py"
    COMMENT "Converting MAVLink headers (common + lisum) to DSDL under ${MAVLINK_DSDL_DIR}"
    VERBATIM
)
add_custom_target(mavlink_dsdl_gen DEPENDS "${MAVLINK_DSDL_DIR}/.mav2dsdl.stamp")

# Nunavut code generation for Cyphal
set(NUNAVUT_OUT_DIR "${CMAKE_BINARY_DIR}/cyphal")


file(GLOB_RECURSE CYPHAL_DSDL_FILES_LOCAL
    "${CMAKE_SOURCE_DIR}/messages/*.dsdl"
    "${CMAKE_SOURCE_DIR}/messages/**/*.dsdl"
)
file(GLOB_RECURSE CYPHAL_DSDL_FILES_MAVLINK
    "${CMAKE_BINARY_DIR}/cyphal/cyphal/messages/mavlink/messages_cyphal_uavcan/**/*.dsdl"
)
file(GLOB_RECURSE CYPHAL_DSDL_FILES_REG
    "${CMAKE_SOURCE_DIR}/messages/cyphal/uavcan/**/*.dsdl"
    "${CMAKE_SOURCE_DIR}/messages/cyphal/reg/**/*.dsdl"
)
set(ALL_DSDL_INPUTS ${CYPHAL_DSDL_FILES_REG} ${CYPHAL_DSDL_FILES_LOCAL} ${CYPHAL_DSDL_FILES_MAVLINK})

add_custom_command(
    OUTPUT "${NUNAVUT_OUT_DIR}/uavcan_stamp"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${NUNAVUT_OUT_DIR}/uavcan"
    COMMAND ${PYTHON_EXECUTABLE} -m nunavut
        --target-language c
        --outdir "${NUNAVUT_OUT_DIR}/uavcan"
        --allow-unregulated-fixed-port-id
        "${CMAKE_SOURCE_DIR}/messages/cyphal/uavcan"
    COMMAND ${CMAKE_COMMAND} -E touch "${NUNAVUT_OUT_DIR}/uavcan_stamp"
    DEPENDS ${CYPHAL_DSDL_FILES_REG}
    COMMENT "Generating regulated UAVCAN headers into ${NUNAVUT_OUT_DIR}/uavcan"
    VERBATIM
)

add_custom_command(
    OUTPUT "${NUNAVUT_OUT_DIR}/reg_stamp"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${NUNAVUT_OUT_DIR}/reg"
    COMMAND ${PYTHON_EXECUTABLE} -m nunavut
        --target-language c
        --outdir "${NUNAVUT_OUT_DIR}/reg"
        --lookup-dir "${CMAKE_SOURCE_DIR}/messages/cyphal/uavcan"
        --allow-unregulated-fixed-port-id
        "${CMAKE_SOURCE_DIR}/messages/cyphal/reg"
    COMMAND ${CMAKE_COMMAND} -E touch "${NUNAVUT_OUT_DIR}/reg_stamp"
    DEPENDS ${CYPHAL_DSDL_FILES_REG}
    COMMENT "Generating regulated reg headers into ${NUNAVUT_OUT_DIR}/reg"
    VERBATIM
)

add_custom_command(
    OUTPUT "${NUNAVUT_OUT_DIR}/mavlink_stamp"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${NUNAVUT_OUT_DIR}/mavlink"
    COMMAND ${PYTHON_EXECUTABLE} -m nunavut
        --target-language c
        --outdir "${NUNAVUT_OUT_DIR}/mavlink"
        --lookup-dir "${CMAKE_SOURCE_DIR}/messages/cyphal/uavcan"
        --lookup-dir "${CMAKE_SOURCE_DIR}/messages/cyphal/reg"
        --allow-unregulated-fixed-port-id
        "${CMAKE_BINARY_DIR}/cyphal/cyphal/messages/mavlink/messages_cyphal_uavcan"
    COMMAND ${CMAKE_COMMAND} -E touch "${NUNAVUT_OUT_DIR}/mavlink_stamp"
    DEPENDS mavlink_dsdl_gen ${ALL_DSDL_INPUTS}
    COMMENT "Generating MAVLink-derived Cyphal headers into ${NUNAVUT_OUT_DIR}/mavlink"
    VERBATIM
)

add_custom_target(cyphal_dsdl ALL DEPENDS
    "${NUNAVUT_OUT_DIR}/uavcan_stamp"
    "${NUNAVUT_OUT_DIR}/reg_stamp"
    "${NUNAVUT_OUT_DIR}/mavlink_stamp"
)
add_dependencies(cyphal_dsdl mavlink_dsdl_gen)
add_dependencies(${EXECUTABLE} cyphal_dsdl)

# Auto-generate raw-payload Cyphal publish helpers for MAVLink-derived messages
set(CYPHAL_MAV_PUB_DIR "${CMAKE_BINARY_DIR}/cyphal/utility")
set(CYPHAL_MAV_PUB_SRC "${CYPHAL_MAV_PUB_DIR}/cyphal_mavlink_publishers.c")
set(CYPHAL_MAV_PUB_HDR "${CYPHAL_MAV_PUB_DIR}/cyphal_mavlink_publishers.h")
add_custom_command(
    OUTPUT "${CYPHAL_MAV_PUB_SRC}" "${CYPHAL_MAV_PUB_HDR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CYPHAL_MAV_PUB_DIR}"
    COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/src/middleware/cyphal/gen_publishers.py" --root "${CMAKE_BINARY_DIR}/cyphal/cyphal/messages/mavlink/messages_cyphal_uavcan" --out "${CYPHAL_MAV_PUB_SRC}"
    DEPENDS cyphal_dsdl "${CMAKE_SOURCE_DIR}/src/middleware/cyphal/gen_publishers.py"
    COMMENT "Generating Cyphal MAVLink publisher helper wrappers"
    VERBATIM
)
add_custom_target(cyphal_mavlink_publishers DEPENDS "${CYPHAL_MAV_PUB_SRC}" "${CYPHAL_MAV_PUB_HDR}")
add_dependencies(${EXECUTABLE} cyphal_mavlink_publishers)
target_sources(${EXECUTABLE} PRIVATE "${CYPHAL_MAV_PUB_SRC}")
target_include_directories(${EXECUTABLE} PRIVATE "${CYPHAL_MAV_PUB_DIR}")


# Auto-generate raw-payload Cyphal publish helpers for regulated UAVCAN messages
set(CYPHAL_UAV_PUB_SRC "${CMAKE_BINARY_DIR}/cyphal/utility/cyphal_uavcan_publishers.c")
set(CYPHAL_UAV_PUB_HDR "${CMAKE_BINARY_DIR}/cyphal/utility/cyphal_uavcan_publishers.h")
add_custom_command(
    OUTPUT "${CYPHAL_UAV_PUB_SRC}" "${CYPHAL_UAV_PUB_HDR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CYPHAL_MAV_PUB_DIR}"
    COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/src/middleware/cyphal/gen_publishers.py" --root "${CMAKE_SOURCE_DIR}/messages/cyphal/uavcan" --out "${CYPHAL_UAV_PUB_SRC}"
    DEPENDS ${CYPHAL_DSDL_FILES_REG} "${CMAKE_SOURCE_DIR}/src/middleware/cyphal/gen_publishers.py"
    COMMENT "Generating Cyphal UAVCAN publisher helper wrappers"
    VERBATIM
)
add_custom_target(cyphal_uavcan_publishers DEPENDS "${CYPHAL_UAV_PUB_SRC}" "${CYPHAL_UAV_PUB_HDR}")
add_dependencies(${EXECUTABLE} cyphal_uavcan_publishers)
target_sources(${EXECUTABLE} PRIVATE "${CYPHAL_UAV_PUB_SRC}")


# Auto-generate raw-payload Cyphal publish helpers for regulated reg.udral messages
set(CYPHAL_REG_PUB_SRC "${CMAKE_BINARY_DIR}/cyphal/utility/cyphal_reg_publishers.c")
set(CYPHAL_REG_PUB_HDR "${CMAKE_BINARY_DIR}/cyphal/utility/cyphal_reg_publishers.h")
add_custom_command(
    OUTPUT "${CYPHAL_REG_PUB_SRC}" "${CYPHAL_REG_PUB_HDR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CYPHAL_MAV_PUB_DIR}"
    COMMAND ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/src/middleware/cyphal/gen_publishers.py" --root "${CMAKE_SOURCE_DIR}/messages/cyphal/reg" --out "${CYPHAL_REG_PUB_SRC}"
    DEPENDS ${CYPHAL_DSDL_FILES_REG} "${CMAKE_SOURCE_DIR}/src/middleware/cyphal/gen_publishers.py"
    COMMENT "Generating Cyphal reg.udral publisher helper wrappers"
    VERBATIM
)
add_custom_target(cyphal_reg_publishers DEPENDS "${CYPHAL_REG_PUB_SRC}" "${CYPHAL_REG_PUB_HDR}")
add_dependencies(${EXECUTABLE} cyphal_reg_publishers)
target_sources(${EXECUTABLE} PRIVATE "${CYPHAL_REG_PUB_SRC}")


add_custom_command(TARGET ${EXECUTABLE}
	POST_BUILD
    COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE}
	COMMAND arm-none-eabi-objcopy -O ihex program.elf program.hex
	COMMAND arm-none-eabi-objcopy -O binary program.elf program.bin
)