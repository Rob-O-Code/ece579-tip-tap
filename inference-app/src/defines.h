#pragma once

#define NDEBUG

#define DBG_SIG_ML 10

#define BUTTON_L 24
#define BUTTON_R 25

#define SENSOR_FSR_PIN 26
#define SENSOR_PIEZO0_PIN 27
#define SENSOR_FSR_PIN_2 28
#define SENSOR_PIEZO1_PIN 29

// There are 64 samples per window
#define ADC_WINDOW_INDEX_BITS 6
#define ADC_WINDOW_COUNT ((1<<ADC_WINDOW_INDEX_BITS))
#define ADC_NUM_WINDOWS 8

#define ADC_BUFFER_WRAP(index) (index & ((1 << ADC_BUFFER_INDEX_BITS) - 1))

#define ADC_SAMPLE_RATE 50*1000
#define ADC_PERIOD 96 * 500*1000 / ADC_SAMPLE_RATE

// constants
#define SAMPLE_RATE       16000
#define FFT_SIZE          128    // ARM_TABLE_TWIDDLECOEF_Q15_64
#define SPECTRUM_WIDTH    5
#define SPECTRUM_SHIFT    1
#define INPUT_BUFFER_SIZE ((FFT_SIZE / 2) * SPECTRUM_SHIFT)
#define INPUT_SHIFT       0