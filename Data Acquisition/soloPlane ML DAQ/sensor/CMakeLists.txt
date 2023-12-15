add_library(sensor INTERFACE)

target_sources(sensor INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/sensor.c
        )

target_include_directories(sensor INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(sensor INTERFACE
        hardware_dma
        hardware_adc)