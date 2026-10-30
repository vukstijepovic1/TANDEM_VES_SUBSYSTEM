## EDePro Motor Example

This example is used to demonstrate the usage of the EDePro Motor driver.

### How to use example

Python script `motor-test.py` is used to generate EDePro Motor packet and send
it to MCU via UART. Each time MCU receives valid EDePro Motor packet green led
is toggled. User can configure UART Instance which shall be used for
communication using `MOTOR_UART_INSTANCE`, communication baud rate using
`MOTOR_UART_BAUD_RATE` and receive queue length using `MOTOR_RX_QUEUE_LEN`.

### How to configure driver

To configure EDePro Motor driver:

1. Create `motor_config.h` file in project `config` folder.
2. Add `MOTOR_CONFIG` compile definition in project CMakeLists.txt

    Project Directory Tree:
    ```
    ./app-shell
    |-- cmake
    |   `-- arm-none-eabi-gcc.cmake
    |-- config
    |   |-- FreeRTOSConfig.h
    |   `-- motor_config.h
    |-- src
    |   |-- app
    |   |   |-- main.c
    |   |   `-- main.h
    |   |-- drivers
    |   |   `-- motor
    |   |       |-- CMakeLists.txt
    |   |       |-- motor_def.h
    |   |       |-- motor.c
    |   |       `-- motor.h
    |-- CMakeLists.txt
    |-- CMakePresets.json
    |-- README.md
    ```

    CMakeLists:
    ``` cmake
    if(EXISTS ${LSM_SOURCE_DIR}/config/motor_config.h)
        add_compile_definitions(MOTOR_CONFIG)
    endif()
    ```
3. Configure arguments such as:
    - **MOTOR_DBG**