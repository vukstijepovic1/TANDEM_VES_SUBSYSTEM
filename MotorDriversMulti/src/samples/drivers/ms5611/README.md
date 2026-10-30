## MS5611 Example

Reads data from MS5611 barometer.

This example is used to demonstrate the usage of the MS5611 driver, which can
communicate with sensor either with I2C or SPI.

### How to use example

At the top of the `main.c` file, in `Defines` section, user can choose physical
interface used to communicate with the sensor (I2C or SPI) by uncommenting
desired interface and commenting out others. Please note that if both interfaces are uncommented example will default to SPI.

### SPI

User can declare SPI Instance which shall be used for communication using
`MS5611_SPI_INSTANCE`, please note that LPC only has one SPI instance.
Additionally user can also define chip select port and pin using
`MS5611_CS_PORT` and `MS5611_CS_PIN`.

```
     LPC1768                          MS5611
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
`MS5611_I2C_INSTANCE`.

```
     LPC1768                          MS5611
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

To configure MS5611 driver:

1. Create `ms5611_config.h` file in project `config` folder.
2. Add `MS5611_CONFIG` compile definition in project CMakeLists.txt

    Project Directory Tree:
    ```
    ./app-shell
    |-- cmake
    |   `-- arm-none-eabi-gcc.cmake
    |-- config
    |   |-- FreeRTOSConfig.h
    |   `-- ms5611_config.h
    |-- src
    |   |-- app
    |   |   |-- main.c
    |   |   `-- main.h
    |   |-- drivers
    |   |   `-- ms5611
    |   |       |-- CMakeLists.txt
    |   |       |-- ms5611_common.h
    |   |       |-- ms5611_def.h
    |   |       |-- ms5611_i2c.c
    |   |       |-- ms5611_i2c.h
    |   |       |-- ms5611_spi.c
    |   |       |-- ms5611_spi.h
    |   |       |-- ms5611.c
    |   |       `-- ms5611.h
    |-- CMakeLists.txt
    |-- CMakePresets.json
    |-- README.md
    ```

    CMakeLists:
    ``` cmake
    if(EXISTS ${LSM_SOURCE_DIR}/config/ms5611_config.h)
        add_compile_definitions(MS5611_CONFIG)
    endif()
    ```
3. Configure arguments such as:
    - ***MS5611_I2C_TIMEOUT_MS***
    - ***MS5611_SPI_TIMEOUT_MS***