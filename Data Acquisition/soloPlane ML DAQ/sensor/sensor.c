
#include <stdio.h>
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "sensor.h"
#include "math.h"

int data_chan;
//int ctrl_chan;

uint16_t data_a[ADC_BUFFER_COUNT] __attribute__((aligned(ADC_BUFFER_COUNT)));
uint16_t data_b[ADC_BUFFER_COUNT] __attribute__((aligned(ADC_BUFFER_COUNT)));
uint16_t* data_active = data_a;
uint16_t* data_inactive = data_b;

state_t g_state = SETUP;

queue_t window_fifo;
typedef uint16_t* element_t;

bool midi_trigger = 0;

void sensor_data_handler() {
    if (!queue_try_add(&window_fifo, &data_active)) {
        printf("The window fifo filled up...\n");
    }
    uint16_t* swap = data_inactive;
    data_inactive = data_active;
    data_active = swap;
    dma_channel_acknowledge_irq0(data_chan);
    dma_channel_set_write_addr(data_chan, data_active, false);
    dma_channel_set_trans_count(data_chan, ADC_BUFFER_COUNT, true);
}

static inline void sensor_adc_init() {
    // Init GPIO for analogue use: hi-Z, no pulls, disable digital input buffer.
    adc_gpio_init(SENSOR_PIEZO0_PIN);
    adc_gpio_init(SENSOR_PIEZO1_PIN);

    adc_init();
    adc_select_input(SENSOR_PIEZO0_PIN - 26);
    adc_set_round_robin(0b01010);
    adc_fifo_setup(
        true,    // Write each completed conversion to the sample FIFO
        true,    // Enable DMA data request (DREQ)
        1,       // DREQ (and IRQ) asserted when at least 1 sample present
        false,   // We won't see the ERR bit because of 8 bit reads; disable.
        false    // Don't shift each sample to 8 bits when pushing to FIFO
    );

    // adc_set_clkdiv(96 * 5 / 4); // 500kHz sample rate max
    //                             // 400kHz total sample rate
    //                             // 100kHz per channel
    adc_set_clkdiv(ADC_PERIOD);
}

static inline void sensor_dma_init() {
    data_chan = dma_claim_unused_channel(true);

    dma_channel_config cfg = dma_channel_get_default_config(data_chan);

    // Reading from constant address, writing to ring buffer of size CAPTURE_DEPTH
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);

    // Pace transfers based on availability of ADC samples
    channel_config_set_dreq(&cfg, DREQ_ADC);

    dma_channel_configure(data_chan, &cfg,
        data_active,    // dst
        &adc_hw->fifo,  // src
        0,  // transfer count
        false            // start immediately
    );

    dma_channel_set_irq0_enabled(data_chan, true);
    irq_set_exclusive_handler(DMA_IRQ_0, sensor_data_handler);
    irq_set_enabled(DMA_IRQ_0, true);
}

void sensor_init() {
    queue_init(&window_fifo, sizeof(element_t), 2);
    sensor_adc_init();
    sleep_ms(200);
    sensor_dma_init();
    sleep_ms(200);
    dma_channel_set_trans_count(data_chan, ADC_BUFFER_COUNT, true);
    adc_select_input(SENSOR_PIEZO0_PIN - 26);
    adc_run(true);

    g_state = READY;
}

void trigger(uint16_t* current_window) {
    adc_run(false);
    adc_fifo_drain();

    g_state = WRITING;

    printf("DATA\n");
    for (uint16_t i = 0; i < ADC_BUFFER_COUNT; i++) {
        printf("%d\n", current_window[i]);
    }
    printf("DONE\n");

    dma_channel_set_write_addr(data_chan, data_active, false);
    dma_channel_set_trans_count(data_chan, ADC_BUFFER_COUNT, true);
    adc_select_input(SENSOR_PIEZO0_PIN - 26);
    adc_run(true);

    g_state = READY;
}

void sensor_task() {
    sensor_init();
    volatile uint32_t rms;
    while (1) {
        uint16_t* current_window;
        while (!queue_try_remove(&window_fifo, &current_window)) {
            tight_loop_contents();
        }

        uint32_t sum = 0;
        for (uint16_t i = 0; i < ADC_BUFFER_COUNT; i++) {
            sum += current_window[i];
        }
        uint32_t mean = sum / ADC_BUFFER_COUNT;

        rms = 0;
        for (uint16_t i = 2000; i < ADC_BUFFER_COUNT-2000; i++) {
            int32_t current = (int32_t) current_window[i] - mean;
            current = current * current;
            rms += current / 16;
        }
        // printf("sum: %10d, mean: %10d, rms: %10d\n", sum, mean, rms);
        if (rms > 9000) {
            trigger(current_window);
        }
    }
}