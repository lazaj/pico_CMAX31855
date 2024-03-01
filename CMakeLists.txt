# The console library
add_library(pico_CMAX31855 pico_CMAX31855.h pico_CMAX31855.c)
target_link_libraries(pico_CMAX31855 pico_stdlib hardware_spi)
target_include_directories(pico_CMAX31855 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")