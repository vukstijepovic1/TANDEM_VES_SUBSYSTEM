## SBUS Example

This example is used to demonstrate the usage of the SBUS library.

### How to use example

Python script `sbus-test.py` is used to generate SBUS packet and send it to MCU
via UART. Each time MCU receives valid SBUS packet green led is toggled.

### How to configure library

To configure SBUS Library:

1. Create `sbus_config.h` file in project `config` folder.
2. Add `SBUS_CONFIG` compile definition in project CMakeLists.txt

    Project Directory Tree:
    ```
    ./app-shell
    |-- cmake
    |   `-- arm-none-eabi-gcc.cmake
    |-- config
    |   |-- FreeRTOSConfig.h
    |   `-- sbus_config.h
    |-- src
    |   |-- app
    |   |   |-- main.c
    |   |   `-- main.h
    |   |-- libs
    |   |   `-- sbus
    |   |       |-- CMakeLists.txt
    |   |       |-- sbus.c
    |   |       `-- sbus.h
    |-- CMakeLists.txt
    |-- CMakePresets.json
    |-- README.md
    ```

    CMakeLists:
    ``` cmake
    if(EXISTS ${LSM_SOURCE_DIR}/config/sbus_config.h)
        add_compile_definitions(SBUS_CONFIG)
    endif()
    ```
3. Configure arguments such as:
    - **SBUS_DBG**
    - **SBUS_PACKET_TIMEOUT_MS**