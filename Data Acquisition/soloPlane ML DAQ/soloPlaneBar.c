#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "pico/platform.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "ws2812.h"
#include "sensor.h"
#include "device.h"

#define BUTTON_L 24
#define BUTTON_R 25

extern state_t g_state;

void main1();

int main() {
    stdio_init_all();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    ws2812_init();
    
    // Initialize buttons
    #if BREADBOARD_PIN_MODE
    gpio_init(BUTTON_L);
    gpio_set_dir(BUTTON_L, GPIO_IN);
    gpio_pull_up(BUTTON_L);

    gpio_init(BUTTON_R);
    gpio_set_dir(BUTTON_R, GPIO_IN);
    gpio_pull_up(BUTTON_R);
    #endif
    
    // Launch task on core1 (test)
    multicore_launch_core1(sensor_task);

    sleep_ms(200);

    main1();
    return 0;
}

void main1() {
    sleep_ms(200);
    bool on = true;
    while (1) {
        gpio_put(PICO_DEFAULT_LED_PIN, on);
        sleep_ms(100);

        switch (g_state) {
            case SETUP:
                put_pixel(urgb_u32(127, 16, 16));
                break;
            case READY:
                put_pixel(urgb_u32(16, 127, 16));
                break;
            case WRITING:
                put_pixel(urgb_u32(64, 64, 64));
                break;
        }

        on = !on;
    }
}