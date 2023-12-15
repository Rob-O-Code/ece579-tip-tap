#include "hardware/i2c.h"
#include "pico/i2c_slave.h"
#include "pico/stdlib.h"

#include "databus.h"

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    switch (event) {
    case I2C_SLAVE_RECEIVE: // master has written some data
        uint8_t byte = i2c_read_byte_raw(I2C_S_PORT);
    case I2C_SLAVE_REQUEST: // master is requesting data
        // load from memory
        i2c_write_byte_raw(I2C_S_PORT, 0x68);
        break;
    case I2C_SLAVE_FINISH: // master has signalled Stop / Restart
        break;
    default:
        break;
    }
}

void databus_init() {
    gpio_init(I2C_TX_BLOCK_PIN);
    gpio_set_dir(I2C_TX_BLOCK_PIN, GPIO_OUT);
    gpio_put(I2C_TX_BLOCK_PIN, 0);

    // Initialize I2C
    gpio_set_function(I2C_M_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_M_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_M_SDA);
    gpio_pull_up(I2C_M_SCL);
    
    i2c_init(I2C_M_PORT, I2C_FREQUENCY);

    gpio_set_function(I2C_S_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_S_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_S_SDA);
    gpio_pull_up(I2C_S_SCL);
    
    i2c_init(I2C_S_PORT, I2C_FREQUENCY);

    i2c_slave_init(I2C_S_PORT, I2C_SELF_ADDR, &i2c_slave_handler);
}

void databus_send_to_host(const uint8_t* src, size_t len) {
    i2c_write_timeout_us(I2C_M_PORT, I2C_HOST_ADDR, src, len, false, 10000);
}