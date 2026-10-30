## Daly BMS Example

This example is used to demonstrate the usage of the Daly BMS driver.

### How to use example

Python script `bms-test.py` is used to generate Daly BMS packet and send it to
MCU via UART. Each time MCU receives valid Daly BMS packet green led is toggled.
User can configure UART Instance which shall be used for communication using
`BMS_UART_INSTANCE`, communication baud rate using `BMS_UART_BAUD_RATE` and
receive queue length using `BMS_RX_QUEUE_LEN`.

### How to configure driver

To configure Daly BMS driver:

1. Create `bms_config.h` file in project `config` folder.
2. Add `BMS_CONFIG` compile definition in project CMakeLists.txt

    Project Directory Tree:
    ```
    ./app-shell
    |-- cmake
    |   `-- arm-none-eabi-gcc.cmake
    |-- config
    |   |-- FreeRTOSConfig.h
    |   `-- bms_config.h
    |-- src
    |   |-- app
    |   |   |-- main.c
    |   |   `-- main.h
    |   |-- drivers
    |   |   `-- bms
    |   |       |-- bms_def.h
    |   |       |-- bms.c
    |   |       |-- bms.h
    |   |       `-- CMakeLists.txt
    |-- CMakeLists.txt
    |-- CMakePresets.json
    |-- README.md
    ```

    CMakeLists:
    ``` cmake
    if(EXISTS ${LSM_SOURCE_DIR}/config/bms_config.h)
        add_compile_definitions(BMS_CONFIG)
    endif()
    ```
3. Configure arguments such as:
    - **BMS_DBG**