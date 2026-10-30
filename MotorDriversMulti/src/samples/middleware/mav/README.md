## MAVLink Example

This example is used to demonstrate the usage of the MAVLink library. Two
devices exchange MAVLink heartbeats between each other via multiple redundant
interfaces (2 UDP sockets and 1 UART), each time device receives MAVLink
heartbeat a green LED is toggled.

### How to use example

This example requires two microcontrollers which are connected with both UART
and UTP cable.

Example can be configured to transmit signed or unsigned MAVLink messages.
`MAV_SIGN_ENABLE` can be used to enable/disable transmission of signed messages.
Please note that if unsigned messages are used there is no mechanism to
deduplicate message, and this is shown when `recv_cnt` variable is observed.
In case of unsigned messages `recv_cnt` is incremented by 3 each time heartbeat
messages is received (same messages is received three times), while in case of
signed messagess `recv_cnt` is incremented only by 1 (duplicated messages are
discarded). User can also configure `MAV_UART_INSTANCE` & `MAV_UART_BAUD_RATE`.

### How to configure library

To configure MAVLink library:

1. Create `mav_config.h` file in project `config` folder.
2. Add `MAV_CONFIG` compile definition in project CMakeLists.txt

    Project Directory Tree:
    ```
    ./app-shell
    |-- cmake
    |   `-- arm-none-eabi-gcc.cmake
    |-- config
    |   |-- FreeRTOSConfig.h
    |   `-- mav_config.h
    |-- src
    |   |-- app
    |   |   |-- main.c
    |   |   `-- main.h
    |   |-- middleware
    |   |   `-- mav
    |   |       |-- mavlink
    |   |       |-- CMakeLists.txt
    |   |       |-- mav.c
    |   |       |-- mav.h
    |   |       |-- mavlink_wrapper.c
    |   |       `-- mavlink_wrapper.h
    |-- CMakeLists.txt
    |-- CMakePresets.json
    |-- README.md
    ```

    CMakeLists:
    ``` cmake
    if(EXISTS ${LSM_SOURCE_DIR}/config/mav_config.h)
        add_compile_definitions(MAV_CONFIG)
    endif()
    ```
3. Configure arguments such as:
    - **MAVLINK_COMM_NUM_BUFFERS**
    - **MAVLINK_MAX_SIGNING_STREAMS**
    - **MAV_BUFFER_SIZE**
    - **MAV_MAX_LINK_CNT**
    - **MAV_MAX_TRACK_CNT**