#pragma once
#include "device.h"

#define I2C_M_SDA 2
#define I2C_M_SCL 3

#define I2C_S_SDA (BREADBOARD_PIN_MODE ? 8 : 4)
#define I2C_S_SCL (BREADBOARD_PIN_MODE ? 9 : 5)

#define I2C_TX_BLOCK_PIN (BREADBOARD_PIN_MODE ? 12 : 7)

#define SENSOR_FSR_PIN 26
#define SENSOR_PIEZO0_PIN 27
#define SENSOR_FSR_PIN_2 28
#define SENSOR_PIEZO1_PIN 29

#if !BREADBOARD_PIN_MODE
#define FSR_POWER_PIN 24
#endif