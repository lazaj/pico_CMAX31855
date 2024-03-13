# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C   standards
project(max31855_example_1 C CXX ASM)
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
add_executable(${PROJECT_NAME} 
    ${CMAKE_CURRENT_LIST_DIR}/examples/example_1.c
)

# pull in common dependencies and additional spi hardware support
target_link_libraries(max31855_example_1 pico_stdlib PICO_MAX31855)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME} 
    pico_stdlib
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)