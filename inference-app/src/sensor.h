#pragma once

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

#include "tflite_model.h"
#include "dsp_pipeline.h"
#include "ml_model.h"
#include <stdio.h>
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "math.h"

#include "defines.h"

int data_chan;

uint16_t data[ADC_NUM_WINDOWS][ADC_WINDOW_COUNT] __attribute__((aligned(ADC_WINDOW_COUNT)));

queue_t frame_fifo;
typedef uint16_t* element_t;

uint8_t cur_window = 0;

void sensor_data_handler() {
    dma_channel_acknowledge_irq1(data_chan);
    uint16_t* data_pointer = data[cur_window];
    if (!queue_try_add(&frame_fifo, &data_pointer)) {
        printf("The window fifo filled up...\n");
    }
    //frame = (uint16_t *) dma_channel_hw_addr(data_chan)->write_addr;
    if (++cur_window >= ADC_NUM_WINDOWS) cur_window = 0;
    dma_channel_set_write_addr(data_chan, data[cur_window], false);
    dma_channel_set_trans_count(data_chan, ADC_WINDOW_COUNT, true);
}

static inline void sensor_adc_init() {
    // Init GPIO for analogue use: hi-Z, no pulls, disable digital input buffer.
    adc_gpio_init(SENSOR_PIEZO0_PIN);
    adc_gpio_init(SENSOR_PIEZO1_PIN);

    adc_init();
    adc_select_input(SENSOR_PIEZO0_PIN);
    adc_set_round_robin(0b00010);
    adc_fifo_setup(
        true,    // Write each completed conversion to the sample FIFO
        true,    // Enable DMA data request (DREQ)
        1,       // DREQ (and IRQ) asserted when at least 1 sample present
        false,   // We won't see the ERR bit because of 8 bit reads; disable.
        false    // Don't shift each sample to 8 bits when pushing to FIFO
    );

    adc_set_clkdiv(ADC_PERIOD);
}

static inline void sensor_dma_init() {
    data_chan = dma_claim_unused_channel(true);

    dma_channel_config cfg = dma_channel_get_default_config(data_chan);

    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);

    channel_config_set_dreq(&cfg, DREQ_ADC);

    dma_channel_configure(data_chan, &cfg,
        data[0],    // dst
        &adc_hw->fifo,  // src
        0,  // transfer count
        false            // start immediately
    );

    dma_channel_set_irq1_enabled(data_chan, true);
    irq_set_exclusive_handler(DMA_IRQ_1, sensor_data_handler);
    irq_set_enabled(DMA_IRQ_1, true);
}

void sensor_init() {
    queue_init(&frame_fifo, sizeof(element_t), 2);
    sensor_adc_init();
    sleep_ms(200);
    sensor_dma_init();
    sleep_ms(200);
    dma_channel_set_trans_count(data_chan, ADC_WINDOW_COUNT, true);
    adc_select_input(SENSOR_PIEZO0_PIN - 26);
    adc_run(true);
}

q15_t capture_buffer_q15[INPUT_BUFFER_SIZE];
volatile int new_samples_captured = 0;

q15_t input_q15[INPUT_BUFFER_SIZE + (FFT_SIZE / 2)];

DSPPipeline dsp_pipeline(FFT_SIZE);
MLModel ml_model(tflite_model, 128 * 1024);

int8_t* scaled_spectrum = nullptr;
int32_t spectogram_divider;
float spectrogram_zero_point;

uint16_t* current_frame;
uint16_t* previous_frame;

bool trigger = false;
uint8_t countdown = 0;

int8_t full_spectrum[65];
int16_t full_wave[64];
uint32_t rms = 0;
void sensor_task() {
    while (!queue_try_remove(&frame_fifo, &current_frame)) {
        tight_loop_contents();
    }

    dsp_pipeline.shift_spectrogram(scaled_spectrum, SPECTRUM_SHIFT, SPECTRUM_WIDTH);

    // move input buffer values over by INPUT_BUFFER_SIZE samples
    memmove(input_q15, &input_q15[INPUT_BUFFER_SIZE], (FFT_SIZE / 2)*2);

    rms = 0;
    for (uint16_t i = 0; i < FFT_SIZE/2; i++) {
        full_wave[i] = (int16_t) current_frame[i] - 2047;
        // sample >>= 4;
        input_q15[INPUT_BUFFER_SIZE + i] = current_frame[i] * 96; // 64 was okay
        rms += full_wave[i] * full_wave[i];
    }
    // arm_shift_q15(full_wave, 0, input_q15 + (FFT_SIZE / 2), INPUT_BUFFER_SIZE);

    //const int16_t* input_read = input_q15 + ((FFT_SIZE / 2));
    //int8_t* stft_write = scaled_spectrum + (65 * (5 - SPECTRUM_SHIFT));
    dsp_pipeline.calculate_spectrum(
        input_q15, full_spectrum,
        spectogram_divider, spectrogram_zero_point
    );

    for (uint8_t i = 0; i < 8; i++) {
        scaled_spectrum[(8 * (5 - SPECTRUM_SHIFT)) + i] = full_spectrum[i];
    }

    if (countdown == 0) {
        if (rms > 8000) countdown = 2;
    } else if (countdown == 1) {
        countdown = 0;
        trigger = true;
    } else {
        countdown--;
    }
}