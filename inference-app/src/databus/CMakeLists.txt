add_library(databus INTERFACE)

target_sources(databus INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/databus.c
        )

target_include_directories(databus INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(databus INTERFACE hardware_i2c pico_i2c_slave)