#pragma once

#define CLIENT 1
#define BREADBOARD_PIN_MODE 1

#define I2C_M_SDA 2
#define I2C_M_SCL 3

#define I2C_S_SDA (BREADBOARD_PIN_MODE ? 8 : 4)
#define I2C_S_SCL (BREADBOARD_PIN_MODE ? 9 : 5)

#define I2C_TX_BLOCK_PIN (BREADBOARD_PIN_MODE ? 12 : 7)

#define I2C_FREQUENCY 400*1000
#define I2C_M_PORT i2c1
#define I2C_S_PORT i2c0

#define I2C_HOST_ADDR 0x77
#define I2C_CLIENT0_ADDR 0x20
#define I2C_CLIENT1_ADDR 0x21

#define I2C_SELF_ADDR (CLIENT ? I2C_CLIENT1_ADDR : I2C_CLIENT0_ADDR)

void databus_init();

void databus_send_to_host(const uint8_t* src, size_t len);