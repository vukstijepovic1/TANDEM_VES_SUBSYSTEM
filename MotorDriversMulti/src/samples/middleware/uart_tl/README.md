## UART Transport Layer Example

This example is used to demonstrate the usage of the UART Transport Layer.
Example is built upon previous example given for SBUS, showcasing the usage and
advantages of using UART Transport Layer.

### How to use example

Python script `uart-tl-test.py` is used to generate SBUS packet and send it to
MCU via UART. Each time MCU receives valid SBUS packet green led is toggled and
packet is echoed back to the python script via UART.

### How to configure library

To configure UART Transport Layer.:

1. Create `uart_tl_config.h` file in project `config` folder.
2. Add `UART_TL_CONFIG` compile definition in project CMakeLists.txt

    Project Directory Tree:
    ```
    ./app-shell
    |-- cmake
    |   `-- arm-none-eabi-gcc.cmake
    |-- config
    |   |-- FreeRTOSConfig.h
    |   `-- uart_tl_config.h
    |-- src
    |   |-- app
    |   |   |-- main.c
    |   |   `-- main.h
    |   |-- middleware
    |   |   `-- tl
    |   |       |-- uart_tl
    |   |       |   |-- CMakeLists.txt
    |   |       |   |-- uart_tl.c
    |   |       |   `-- uart_tl.h
    |   |       |-- CMakeLists.txt
    |   |       |-- tl_common.c
    |   |       `-- tl_common.h
    |-- CMakeLists.txt
    |-- CMakePresets.json
    |-- README.md
    ```

    CMakeLists:
    ``` cmake
    if(EXISTS ${LSM_SOURCE_DIR}/config/uart_tl_config.h)
        add_compile_definitions(UART_TL_CONFIG)
    endif()
    ```
3. Configure arguments such as:
    - **UART_TL_DBG**