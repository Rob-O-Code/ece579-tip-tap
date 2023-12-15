#pragma once

#include "pins.h"

#define ADC_BUFFER_INDEX_BITS 13
#define ADC_BUFFER_COUNT (1<<ADC_BUFFER_INDEX_BITS)

#define ADC_BUFFER_WRAP(index) (index & ((1 << ADC_BUFFER_INDEX_BITS) - 1))

#define ADC_SAMPLE_RATE (100*1000)

#define ADC_PERIOD (96 * 500*1000 / ADC_SAMPLE_RATE)


typedef enum state_t {
    SETUP,
    WRITING,
    READY
} state_t;

void sensor_task();
