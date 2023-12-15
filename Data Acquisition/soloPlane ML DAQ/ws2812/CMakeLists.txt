add_library(ws2812 INTERFACE)

target_sources(ws2812 INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/ws2812.c
        )

target_include_directories(ws2812 INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_BINARY_DIR})

file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)

# generate the header file into the source tree as it is included in the RP2040 datasheet
pico_generate_pio_header(ws2812 ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

target_link_libraries(ws2812 INTERFACE pico_stdlib hardware_pio)
# pico_add_extra_outputs(ws2812)

# Additionally generate python and hex pioasm outputs for inclusion in the RP2040 datasheet
# add_custom_target(ws2812_datasheet DEPENDS ${CMAKE_CURRENT_LIST_DIR}/generated/ws2812.py)
# add_custom_command(OUTPUT ${CMAKE_CURRENT_LIST_DIR}/generated/ws2812.py
#         DEPENDS ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio
#         COMMAND Pioasm -o python ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio ${CMAKE_CURRENT_LIST_DIR}/generated/ws2812.py
#         VERBATIM)
# add_dependencies(ws2812 ws2812_datasheet)