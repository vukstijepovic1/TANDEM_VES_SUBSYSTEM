## MS5607 Example

Reads data from MS5607 barometer.

This example is used to demonstrate the usage of the MS5607 driver, which can
communicate with sensor either with I2C or SPI.

### How to use example

At the top of the `main.c` file, in `Defines` section, user can choose physical
interface used to communicate with the sensor (I2C or SPI) by uncommenting
desired interface and commenting out others. Please note that if both interfaces are uncommented example will default to SPI.

### SPI

User can declare SPI Instance which shall be used for communication using
`MS5607_SPI_INSTANCE`, please note that LPC only has one SPI instance.
Additionally user can also define chip select port and pin using
`MS5607_CS_PORT` and `MS5607_CS_PIN`.

```
     LPC1768                          MS5607
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
+---------------+               +---------------+
```

### I2C

User can declare I2C Instance which shall be used for communication using
`MS5607_I2C_INSTANCE`.

```
     LPC1768                          MS5607
+---------------+               +---------------+
|           GND +---------------+ GND           |
|               |               |               |
|           3V3 +---------------+ 3V3           |
|               |               |               |
|           SDA +---------------+ SDA           |
|               |               |               |
|           SCL +---------------+ SCL           |
+---------------+               +---------------+
```

### How to configure driver

To configure MS5607 driver:

1. Create `ms5607_config.h` file in project `config` folder.
2. Add `MS5607_CONFIG` compile definition in project CMakeLists.txt

    Project Directory Tree:
    ```
    ./app-shell
    |-- cmake
    |   `-- arm-none-eabi-gcc.cmake
    |-- config
    |   |-- FreeRTOSConfig.h
    |   `-- ms5607_config.h
    |-- src
    |   |-- app
    |   |   |-- main.c
    |   |   `-- main.h
    |   |-- drivers
    |   |   `-- ms5607
    |   |       |-- CMakeLists.txt
    |   |       |-- ms5607_common.h
    |   |       |-- ms5607_def.h
    |   |       |-- ms5607_i2c.c
    |   |       |-- ms5607_i2c.h
    |   |       |-- ms5607_spi.c
    |   |       |-- ms5607_spi.h
    |   |       |-- ms5607.c
    |   |       `-- ms5607.h
    |-- CMakeLists.txt
    |-- CMakePresets.json
    |-- README.md
    ```

    CMakeLists:
    ``` cmake
    if(EXISTS ${LSM_SOURCE_DIR}/config/ms5607_config.h)
        add_compile_definitions(MS5607_CONFIG)
    endif()
    ```
3. Configure arguments such as:
    - ***MS5607_I2C_TIMEOUT_MS***
    - ***MS5607_SPI_TIMEOUT_MS***