/*
 * Robbie Oleynick
 * Worcester Polytechnic Institute
 * ECE 579: On-Device Deep Learning
 * 
 * Adaptation of:
 * ML-Audio-Classifier-Example-for-Pico
 * Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 * 
 */

#include <stdio.h>

#include "pico/stdlib.h"
// #include "pico/multicore.h"

extern "C" {
#include "databus.h"
}
#include "sensor.h"

uint8_t msg[4] = {0xab, I2C_SELF_ADDR, 63, 0};

int main() {
    stdio_init_all();

    printf("hello pico mallet detection\n");

    if (!ml_model.init()) {
        printf("Failed to initialize ML model!\n");
        while (1) { tight_loop_contents(); }
    }

    if (!dsp_pipeline.init()) {
        printf("Failed to initialize DSP Pipeline!\n");
        while (1) { tight_loop_contents(); }
    }

    scaled_spectrum = (int8_t*)ml_model.input_data();
    spectogram_divider = 64 * ml_model.input_scale(); 
    spectrogram_zero_point = ml_model.input_zero_point();

    gpio_init(BUTTON_L);
    gpio_set_dir(BUTTON_L, GPIO_IN);
    gpio_pull_up(BUTTON_L);

    gpio_init(DBG_SIG_ML);
    gpio_set_dir(DBG_SIG_ML, GPIO_OUT);
    gpio_put(DBG_SIG_ML, 0);

    databus_init();
    sensor_init();

    while (1) {
        sensor_task();

        if (trigger || !gpio_get(BUTTON_L)) {
            adc_run(false);
            adc_fifo_drain();

            gpio_put(DBG_SIG_ML, 1);
            volatile float prediction = ml_model.predict();
            gpio_put(DBG_SIG_ML, 0);

            if (prediction >= 0.75) {
                printf("Plastic detected!\t(prediction = %f, rms = %d)\n\n", prediction, rms);
                msg[3] = 2;
            } else {
                printf("SoftYrn detected!\t(prediction = %f, rms = %d)\n\n", prediction, rms);
                msg[3] = 1;
            }
            gpio_put(I2C_TX_BLOCK_PIN, 1);
            databus_send_to_host(msg, 4);
            gpio_put(I2C_TX_BLOCK_PIN, 0);

            dma_channel_set_write_addr(data_chan, data[cur_window], false);
            dma_channel_set_trans_count(data_chan, ADC_WINDOW_COUNT, true);
            adc_select_input(SENSOR_PIEZO0_PIN - 26);
            while (!gpio_get(BUTTON_L))
                tight_loop_contents();
            trigger = false;
            adc_run(true);
        }
    }

    return 0;
}
