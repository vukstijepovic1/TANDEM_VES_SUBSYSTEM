## ANPP Example

This example is used to demonstrate the usage of the ANPP driver.

### How to use example

Connect ANPP airspeed sensor via UART and select appropriate UART Instance using
`ANPP_UART_INSTANCE`. To ensure that collected data is from the same epoch sync
mechanism is added. Each time that the `task_anpp` collects all of the data from
the epoch 5ms pulse is set on `GPIO1.18`. User can connect oscilloscope probe to
UART RX channel of the MCU and other probe to pin `GPIO1.18`. User can notice
that after each epoch 5ms pulse is seen on `GPIO1.18`. Each sample is started
with ANPP Raw Sensors message, which is stated in datasheet, this is used to
create sync mechanism.

### How to configure driver

To configure ANPP driver:

1. Create `anpp_config.h` file in project `config` folder.
2. Add `ANPP_CONFIG` compile definition in project CMakeLists.txt

    Project Directory Tree:
    ```
    ./app-shell
    |-- cmake
    |   `-- arm-none-eabi-gcc.cmake
    |-- config
    |   |-- FreeRTOSConfig.h
    |   `-- anpp_config.h
    |-- src
    |   |-- app
    |   |   |-- main.c
    |   |   `-- main.h
    |   |-- drivers
    |   |   `-- anpp
    |   |       |-- anpp_common.c
    |   |       |-- anpp_common.h
    |   |       |-- anpp.c
    |   |       |-- anpp.h
    |   |       `-- CMakeLists.txt
    |-- CMakeLists.txt
    |-- CMakePresets.json
    |-- README.md
    ```

    CMakeLists:
    ``` cmake
    if(EXISTS ${LSM_SOURCE_DIR}/config/anpp_config.h)
        add_compile_definitions(ANPP_CONFIG)
    endif()
    ```
3. Configure arguments such as:
    - **ANPP_DBG**
    - **ANPP_MAX_PACKET_LEN**