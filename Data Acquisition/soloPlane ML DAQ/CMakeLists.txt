# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/Users/rpoleynick/pico/pico-sdk")

set(PICO_BOARD adafruit_feather_rp2040 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(soloPlaneBar C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

set(PICO_CXX_ENABLE_RTTI 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(soloPlaneBar soloPlaneBar.c )

pico_set_program_name(soloPlaneBar "soloPlaneBar")
pico_set_program_version(soloPlaneBar "0.1")

pico_enable_stdio_uart(soloPlaneBar 0)
pico_enable_stdio_usb(soloPlaneBar 1)

# Add the standard library to the build
target_link_libraries(soloPlaneBar
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(soloPlaneBar PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(soloPlaneBar
        pico_stdlib
        pico_multicore
        hardware_dma
        hardware_pio
        pico_i2c_slave
        )

add_subdirectory(ws2812)
target_link_libraries(soloPlaneBar ws2812)

add_subdirectory(sensor)
target_link_libraries(soloPlaneBar sensor)

pico_add_extra_outputs(soloPlaneBar)
