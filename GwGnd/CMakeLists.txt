cmake_minimum_required(VERSION 3.20)

project(gw_gnd
    VERSION 1.0.0
    DESCRIPTION "Gateway Ground project."
)

enable_language(
    C
    CXX
    ASM
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
# === Build metadata ===
string(TIMESTAMP BUILD_UNIX_TIMESTAMP "%s" UTC)

execute_process(
    COMMAND git rev-parse --short=8 HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT GIT_TAG)
    set(GIT_TAG "v0.0.0")
endif()

# FreeRTOS version
file(READ ${CMAKE_SOURCE_DIR}/src/middleware/FreeRTOS/manifest.yml FREERTOS_MANIFEST)
string(REGEX MATCH "version:[ \t]*\"([^\"]+)\"" _match "${FREERTOS_MANIFEST}")
set(OS_VERSION "${CMAKE_MATCH_1}")
if(NOT OS_VERSION)
    set(OS_VERSION "v0.0.0")
endif()

# LittleFS version
file(READ ${CMAKE_SOURCE_DIR}/src/middleware/lfs/littlefs/lfs.h LFS_HEADER)
string(REGEX MATCH "#define LFS_VERSION[ \t]+0x[0-9a-fA-F]+" LFS_VERSION_LINE "${LFS_HEADER}")
string(REGEX REPLACE ".*0x" "" LFS_VERSION_BUILD "${LFS_VERSION_LINE}")

# MAVLink (direktno setovane vrednosti)
set(MAVLINK_VERSION 2.0)
set(MAVLINK_DIALECT "common")

# Ispis u terminal
message(STATUS "Git Tag           : ${GIT_TAG}")
message(STATUS "Git Commit Hash   : ${GIT_HASH}")
message(STATUS "Build Timestamp   : ${BUILD_UNIX_TIMESTAMP}")
message(STATUS "FreeRTOS OS_VERSION : ${OS_VERSION}")
message(STATUS "LittleFS Version  : ${LFS_VERSION_BUILD}")
message(STATUS "MAVLink dialect   : ${MAVLINK_DIALECT}")
message(STATUS "MAVLink version   : ${MAVLINK_VERSION}")

# Konfiguracija header-a
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
add_subdirectory(src/drivers/ms5607 ${LSM_BINARY_DIR}/src/drivers/ms5607)
add_subdirectory(src/drivers/ms5611 ${LSM_BINARY_DIR}/src/drivers/ms5611)
add_subdirectory(src/drivers/sd ${LSM_BINARY_DIR}/src/drivers/sd)
add_subdirectory(src/drivers/ubx ${LSM_BINARY_DIR}/src/drivers/ubx)

# =============================================================
#	libs
# =============================================================
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

# ==============================================================================
#	Add Subdirectories
# ==============================================================================
set(PROJECT_SOURCES
    devices/${DEVICE}/startup.S
    src/app/main.c
    src/app/task_mav.c
    src/app/task_work.c
)

set(EXECUTABLE program.elf)
add_executable(${EXECUTABLE} ${PROJECT_SOURCES})

target_include_directories(${EXECUTABLE}
PUBLIC
    src/app
)

target_compile_options(${DEVICE}
PRIVATE
	-w
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
    ms5607
    ms5611
    sd
    ubx

    # libs
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
)

add_custom_command(TARGET ${EXECUTABLE}
	POST_BUILD
    COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE}
	COMMAND arm-none-eabi-objcopy -O ihex program.elf program.hex
	COMMAND arm-none-eabi-objcopy -O binary program.elf program.bin
)