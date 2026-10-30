## ICM42688P Example

Reads data from ICM42688P accelerometer and barometer.

This example is used to demonstrate the usage of the ICM42688P driver, which
uses SPI interface. Green and red led are synced while sensor data is read
correctly.

### How to use example

At the top of the `main.c` file, in `Defines` section, user can configure SPI
physical interface.

### SPI

User can declare SPI Instance which shall be used for communication using
`ICM42688P_SPI_INSTANCE`, please note that LPC only has one SPI instance.
Additionally user can also define chip select port and pin using
`ICM42688P_CS_PORT` and `ICM42688P_CS_PIN`.

```
     LPC1768                        ICM42688P
+---------------+               +---------------+
|           GND +---------------+ GND           |
|               |               |               |
|           3V3 +---------------+ 3V3           |
|               |               |               |
|            CS +---------------+ CS            |
|               |               |               |
|          MOSI +---------------+ SDI           |
|               |               |               |
|          MISO +---------------+ SDO           |
|               |               |               |
|           SCK +---------------+ SCK           |
|               |               |               |
|           INT +---------------+ INT           |
+---------------+               +---------------+
```

### How to configure driver

To configure ICM42688P driver:

1. Create `icm42688p_config.h` file in project `config` folder.
2. Add `ICM42688P_CONFIG` compile definition in project CMakeLists.txt

    Project Directory Tree:
    ```
    ./app-shell
    |-- cmake
    |   `-- arm-none-eabi-gcc.cmake
    |-- config
    |   |-- FreeRTOSConfig.h
    |   `-- icm42688p_config.h
    |-- src
    |   |-- app
    |   |   |-- main.c
    |   |   `-- main.h
    |   |-- drivers
    |   |   `-- icm42688p
    |   |       |-- CMakeLists.txt
    |   |       |-- icm42688p_common.h
    |   |       |-- icm42688p_def.h
    |   |       |-- icm42688p_spi.c
    |   |       |-- icm42688p_spi.h
    |   |       |-- icm42688p.c
    |   |       `-- icm42688p.h
    |-- CMakeLists.txt
    |-- CMakePresets.json
    |-- README.md
    ```

    CMakeLists:
    ``` cmake
    if(EXISTS ${LSM_SOURCE_DIR}/config/icm42688p_config.h)
        add_compile_definitions(ICM42688P_CONFIG)
    endif()
    ```
3. Configure arguments such as:
    - ***ICM42688P_SPI_TIMEOUT_MS***