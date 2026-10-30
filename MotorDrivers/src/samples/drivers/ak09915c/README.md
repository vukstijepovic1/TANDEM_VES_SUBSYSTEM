## AK09915C Example

Reads data from AK09915C magnetometer.

This example is used to demonstrate the usage of the AK09915C driver, which can
communicate with sensor either with I2C or SPI.

### How to use example

At the top of the `main.c` file, in `Defines` section, user can choose physical
interface used to communicate with the sensor (I2C or SPI) by uncommenting
desired interface and commenting out others. Please note that if both interfaces are uncommented example will default to SPI.

### SPI

User can declare SPI Instance which shall be used for communication using
`AK09915C_SPI_INSTANCE`, please note that LPC only has one SPI instance.
Additionally user can also define chip select port and pin using
`AK09915C_CS_PORT` and `AK09915C_CS_PIN`. Please connect AK09915C RST pin to 3V3
if unused.

```
     LPC1768                         AK09915C
+---------------+               +---------------+
|           GND +---------------+ GND           |
|               |               |               |
|           3V3 +---------------+ 3V3           |
|               |               |               |
|           RST +---------------+ RST           |
|               |               |               |
|           INT +---------------+ INT           |
|               |               |               |
|            CS +---------------+ CS            |
|               |               |               |
|          MOSI +---------------+ SDI           |
|               |               |               |
|          MISO +---------------+ SDO           |
|               |               |               |
|           SCK +---------------+ SCK           |
+---------------+               +---------------+
```

### I2C

User can declare I2C Instance which shall be used for communication using
`AK09915C_I2C_INSTANCE`. Please connect AK09915C RST pin to 3V3 if unused.

```
     LPC1768                         AK09915C
+---------------+               +---------------+
|           GND +---------------+ GND           |
|               |               |               |
|           3V3 +---------------+ 3V3           |
|               |               |               |
|           RST +---------------+ RST           |
|               |               |               |
|           INT +---------------+ INT           |
|               |               |               |
|           SDA +---------------+ SDA           |
|               |               |               |
|           SCL +---------------+ SCL           |
+---------------+               +---------------+
```

### How to configure driver

To configure AK09915C driver:

1. Create `ak09915c_config.h` file in project `config` folder.
2. Add `AK09915C_CONFIG` compile definition in project CMakeLists.txt

    Project Directory Tree:
    ```
    ./app-shell
    |-- cmake
    |   `-- arm-none-eabi-gcc.cmake
    |-- config
    |   |-- FreeRTOSConfig.h
    |   `-- ak09915c_config.h
    |-- src
    |   |-- app
    |   |   |-- main.c
    |   |   `-- main.h
    |   |-- drivers
    |   |   `-- ak09915c
    |   |       |-- CMakeLists.txt
    |   |       |-- ak09915c_common.h
    |   |       |-- ak09915c_def.h
    |   |       |-- ak09915c_i2c.c
    |   |       |-- ak09915c_i2c.h
    |   |       |-- ak09915c_spi.c
    |   |       |-- ak09915c_spi.h
    |   |       |-- ak09915c.c
    |   |       `-- ak09915c.h
    |-- CMakeLists.txt
    |-- CMakePresets.json
    |-- README.md
    ```

    CMakeLists:
    ``` cmake
    if(EXISTS ${LSM_SOURCE_DIR}/config/ak09915c_config.h)
        add_compile_definitions(AK09915C_CONFIG)
    endif()
    ```
3. Configure arguments such as:
    - ***AK09915C_I2C_TIMEOUT_MS***
    - ***AK09915C_SPI_TIMEOUT_MS***