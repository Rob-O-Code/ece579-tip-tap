/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "generated/ws2812.pio.h"
#include "ws2812.h"

#define IS_RGBW true
#define NUM_PIXELS 1
#define WS2812_PIN 16
#define WS2812_POWER 17

int ws2812_sm;

void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, ws2812_sm, pixel_grb << 8u);
}

uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

void ws2812_init() {
    gpio_init(WS2812_POWER);
    gpio_set_dir(WS2812_POWER, GPIO_OUT);
    gpio_set_drive_strength(WS2812_POWER, GPIO_DRIVE_STRENGTH_12MA);
    gpio_put(WS2812_POWER, 1);
    
    PIO pio = pio0;
    ws2812_sm = 0; //pio_claim_unused_sm(pio0, true);
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, ws2812_sm, offset, WS2812_PIN, 800000, IS_RGBW);
}

// int main() {
//     //set_sys_clock_48();
//     stdio_init_all();
//     printf("WS2812 Smoke Test, using pin %d", WS2812_PIN);

//     // todo get free sm
//     PIO pio = pio0;
//     int sm = 0;
//     uint offset = pio_add_program(pio, &ws2812_program);

//     ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

//     int t = 0;
//     while (1) {
//         int pat = rand() % count_of(pattern_table);
//         int dir = (rand() >> 30) & 1 ? 1 : -1;
//         puts(pattern_table[pat].name);
//         puts(dir == 1 ? "(forward)" : "(backward)");
//         for (int i = 0; i < 1000; ++i) {
//             pattern_table[pat].pat(NUM_PIXELS, t);
//             sleep_ms(10);
//             t += dir;
//         }
//     }
// }
