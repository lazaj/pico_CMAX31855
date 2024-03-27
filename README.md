# pico_CMAX31855 SPI Library for RP2040 (Pico C/C++ SDK)

This is a C library for interfacing with the MAX31855 thermocouple-to-digital converter using SPI communication on the Raspberry Pi RP2040 microcontroller unit (MCU) with the Pico C/C++ Software Development Kit (SDK).

## Features

- Read temperature data from the MAX31855 thermocouple-to-digital converter.
- Supports both Celsius and Fahrenheit temperature units.
- Simple and intuitive API for easy integration into your RP2040 projects.
- Error code handling for detecting thermocouple and communication faults.

## Installation

1. Clone this repository or download the library files directly.
2. Copy the `pico_CMAX31855` folder into your RP2040 project's `lib` directory.

## Check Example for Usage

Check the `example` folder in this repository for usage examples.

## Build Examples

To build the examples, follow these steps:

1. Navigate to the root directory of the library.
2. Create a build directory:

    ```bash
    mkdir build
    cd build
    ```

3. Run CMake to generate build files:

    ```bash
    cmake ..
    ```

4. Compile the examples:

    ```bash
    make
    ```

## Documentation

Documentation for the library can be found in the `documentation/html` folder.

## Compatibility

This library is designed to work with the MAX31855 thermocouple-to-digital converter and RP2040 microcontroller unit using the Pico C/C++ SDK. Ensure proper wiring and compatibility before use.

## Credits

This library was developed by [lazaj30@gmail.com](mailto:lazaj30@gmail.com).
Use a little bit of the code from Adafruit.

## License

This library is licensed under the [BSD License](LICENSE). Feel free to modify and distribute it as needed. If you find it helpful, attribution would be appreciated.
