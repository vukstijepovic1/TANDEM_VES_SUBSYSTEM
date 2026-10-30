## UDP Example

This example is used to demonstrate the usage of the UDP library.

### How to use example

Python script `udp-test.py` is used to generate UDP packet and send it to MCU,
each time MCU receives packet green led is toggled and packet is echoed back.
Network traffic can be observed using any monitoring software (i.e. Wireshark).

Example can be configured to send or ommit gratuitous ARP. If gratuitous ARP
is sent, period can also be configured.

Please note that example won't work without definining `UDP_DEV_MAC` and
`UDP_DEV_IP` in `udp_config.h`

### How to configure library

To configure UDP Library:

1. Create `udp_config.h` file in project `config` folder.
2. Add `UDP_CONFIG` compile definition in project CMakeLists.txt

    Project Directory Tree:
    ```
    ./app-shell
    |-- cmake
    |   `-- arm-none-eabi-gcc.cmake
    |-- config
    |   |-- FreeRTOSConfig.h
    |   `-- udp_config.h
    |-- src
    |   |-- app
    |   |   |-- main.c
    |   |   `-- main.h
    |   |-- middleware
    |   |   `-- udp
    |   |       |-- CMakeLists.txt
    |   |       |-- udp_crc.c
    |   |       |-- udp_crc.h
    |   |       |-- udp.c
    |   |       `-- udp.h
    |-- CMakeLists.txt
    |-- CMakePresets.json
    |-- README.md
    ```

    CMakeLists:
    ``` cmake
    if(EXISTS ${LSM_SOURCE_DIR}/config/udp_config.h)
        add_compile_definitions(UDP_CONFIG)
    endif()
    ```
3. Configure arguments such as:
    - **UDP_ENDIANNESS** - 0 for big-endian & 1 for little-endian architectures
    - **UDP_ETH_MAX** - maximum size of ethernet packet 
    - **UDP_ETH_CRC_SEND** - set if ethernet checksum should be appended on send
    - **UDP_ETH_CRC_RECV** - set if ethernet checksum should be validated on
    recv
    - **UDP_ARP_MAX** - maximum number of entries in ARP table
    - **UDP_ARP_GRAT** - set if gratuitous ARP support is enabled
    - **UDP_TRACK_MAX** - maximum number of UDP connections to track
    - **UDP_DEV_MAC** - device MAC address
    - **UDP_DEV_IP** - device IP