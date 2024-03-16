# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C standards
project(max31855_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_library(PICO_MAX31855 INTERFACE)

target_sources(PICO_MAX31855 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/pico_MAX31855.c
)

target_include_directories(PICO_MAX31855 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(PICO_MAX31855 INTERFACE pico_stdlib hardware_spi)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Tell CMake where to find the executable source file
add_executable(max31855_example_spi0
    ${CMAKE_CURRENT_LIST_DIR}/examples/example_spi0.c
)
add_executable(max31855_example_spi1
    ${CMAKE_CURRENT_LIST_DIR}/examples/example_spi1.c
)

# pull in common dependencies and additional spi hardware support
target_link_libraries(max31855_example_spi0 pico_stdlib PICO_MAX31855)
target_link_libraries(max31855_example_spi1 pico_stdlib PICO_MAX31855)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(max31855_example_spi0)
pico_add_extra_outputs(max31855_example_spi1)

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(max31855_example_spi0 
    pico_stdlib
)
target_link_libraries(max31855_example_spi1 
    pico_stdlib
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(max31855_example_spi0 1)
pico_enable_stdio_uart(max31855_example_spi0 0)
pico_enable_stdio_usb(max31855_example_spi1 1)
pico_enable_stdio_uart(max31855_example_spi1 0)