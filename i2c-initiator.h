/*
 * avr-i2c-initiator: An implementation of I2C bit-bang initiator for AVR
 *                    microcontrollers.
 * Copyright (C) 2020 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define i2c_initiator_init(port, sda, scl) \
    i2c_initiator_init_internal(&DDR ## port, &PORT ## port, &PIN ## port, sda, scl)

void i2c_initiator_init_internal(volatile uint8_t *ddr, volatile uint8_t *port,
    volatile uint8_t *pin, uint8_t sda, uint8_t scl);

#ifdef I2C_LOW_LEVEL_API
#   define _I2C_LL
void i2c_initiator_start_condition(void);
void i2c_initiator_stop_condition(void);
bool i2c_initiator_write_byte(uint8_t data);
uint8_t i2c_initiator_read_byte(bool last);
#else
#   define _I2C_LL static
bool i2c_initiator_write(uint8_t slave_address, uint8_t reg_address, uint8_t *data, size_t data_len);
bool i2c_initiator_read(uint8_t slave_address, uint8_t reg_address, uint8_t *data, size_t data_len);
#endif
