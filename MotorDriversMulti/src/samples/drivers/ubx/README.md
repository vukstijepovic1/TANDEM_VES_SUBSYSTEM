## UBX Example

This example is used to demonstrate the usage of the UBX driver.

### How to use example

Connect UBX GNSS receiver via UART and select appropriate UART Instance using
`UBX_UART_INSTANCE`. To ensure that collected data is from the same epoch sync
mechanism is added. This mechanism can be disabled by defining `DISABLE_SYNC`.
Each time that the `task_ubx` collects all of the data from the epoch 5ms pulse
is set on `GPIO1.18`. User can connect oscilloscope probe to UART RX channel of
the MCU and other probe to pin `GPIO1.18`. While sync mechanism is enabled, user
can notice that after each epoch 5ms pulse is seen on `GPIO1.18`. User can
now repeat the experiment with sync mechanism disabled, if MCU drops any of the
packets from the epoch, data will be desynchronized.

Some of the commonly used UBX packets are tracked and their successful parse is
counted, user can more messages if needed.

### How to configure driver

To configure UBX driver:

1. Create `ubx_config.h` file in project `config` folder.
2. Add `UBX_CONFIG` compile definition in project CMakeLists.txt

    Project Directory Tree:
    ```
    ./app-shell
    |-- cmake
    |   `-- arm-none-eabi-gcc.cmake
    |-- config
    |   |-- FreeRTOSConfig.h
    |   `-- ubx_config.h
    |-- src
    |   |-- app
    |   |   |-- main.c
    |   |   `-- main.h
    |   |-- drivers
    |   |   `-- ubx
    |   |       |-- CMakeLists.txt
    |   |       |-- ubx_common.c
    |   |       |-- ubx_common.h
    |   |       |-- ubx_def.h
    |   |       |-- ubx_parser.c
    |   |       `-- ubx_parser.h
    |-- CMakeLists.txt
    |-- CMakePresets.json
    |-- README.md
    ```

    CMakeLists:
    ``` cmake
    if(EXISTS ${LSM_SOURCE_DIR}/config/ubx_config.h)
        add_compile_definitions(UBX_CONFIG)
    endif()
    ```
3. Configure arguments such as:
    - **UBX_DBG**
    - **UBX_MAX_PACKET_LEN**