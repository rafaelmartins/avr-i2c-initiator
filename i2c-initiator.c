/*
 * avr-i2c-initiator: An implementation of I2C bit-bang initiator for AVR
 *                    microcontrollers.
 * Copyright (C) 2020 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#include <stdlib.h>
#include <util/delay.h>
#include "i2c-initiator.h"

#define __delay() _delay_us(5);
#define __delay2() _delay_us(2.5);

static volatile uint8_t *i2c_ddr = NULL;
static volatile uint8_t *i2c_port = NULL;
static volatile uint8_t *i2c_pin = NULL;
static uint8_t i2c_sda;
static uint8_t i2c_scl;


static void
hold_sda()
{
    *i2c_ddr |= (1 << i2c_sda);
    *i2c_port &= ~(1 << i2c_sda);
}


static void
release_sda()
{
    *i2c_ddr &= ~(1 << i2c_sda);
    *i2c_port |= (1 << i2c_sda);
}


static void
hold_scl()
{
    *i2c_ddr |= (1 << i2c_scl);
    *i2c_port &= ~(1 << i2c_scl);
}


static void
release_scl()
{
    *i2c_ddr &= ~(1 << i2c_scl);
    *i2c_port |= (1 << i2c_scl);
    while (!((*i2c_pin) & (1 << i2c_scl)));
}


static void
toggle_scl()
{
    __delay2();
    release_scl();
    __delay();
    hold_scl();
    __delay2();
}


void
i2c_initiator_init_internal(volatile uint8_t *ddr, volatile uint8_t *port,
    volatile uint8_t *pin, uint8_t sda, uint8_t scl)
{
    if (i2c_ddr != NULL)
        return;

    i2c_ddr = ddr;
    i2c_port = port;
    i2c_pin = pin;
    i2c_sda = sda;
    i2c_scl = scl;

    release_sda();
    release_scl();
}


_I2C_LL void
i2c_initiator_start_condition(void)
{
    release_sda();
    release_scl();
    __delay();
    hold_sda();
    __delay();
    hold_scl();
}


_I2C_LL void
i2c_initiator_stop_condition(void)
{
    hold_sda();
    release_scl();
    __delay();
    release_sda();
}


_I2C_LL bool
i2c_initiator_write_byte(uint8_t data)
{
    for (uint8_t i = 0; i < 8; i++) {
        if (data & (1 << (7 - i))) {
            release_sda();
        }
        else {
            hold_sda();
        }
        toggle_scl();
    }

    release_sda();
    release_scl();
    __delay();

    if ((*i2c_pin) & (1 << i2c_sda)) {
        return false;
    }

    hold_scl();
    hold_sda();
    __delay();

    return true;
}


_I2C_LL uint8_t
i2c_initiator_read_byte(bool last)
{
    uint8_t out = 0;

    release_sda();
    __delay();

    for (uint8_t i = 0; i < 8; i++) {
        release_scl();

        // try to read in the middle of the clock cycle
        __delay2();
        if ((*i2c_pin) & (1 << i2c_sda)) {
            out |= (1 << (7 - i));
        }
        __delay2();

        hold_scl();
        __delay();
    }

    if (last) {
        release_sda();
        toggle_scl();
    }
    else {
        hold_sda();
        toggle_scl();
        release_sda();
    }

    return out;
}


#ifndef I2C_LOW_LEVEL_API

bool
i2c_initiator_write(uint8_t slave_address, uint8_t reg_address, uint8_t *data, size_t data_len)
{
    i2c_initiator_start_condition();

    if (!i2c_initiator_write_byte(slave_address << 1)) {
        return false;
    }

    if (!i2c_initiator_write_byte(reg_address)) {
        return false;
    }

    for (size_t i = 0; i < data_len; i++) {
        if (!i2c_initiator_write_byte(data[i])) {
            return false;
        }
    }

    i2c_initiator_stop_condition();

    return true;
}


bool
i2c_initiator_read(uint8_t slave_address, uint8_t reg_address, uint8_t *data, size_t data_len)
{
    if (!i2c_initiator_write(slave_address, reg_address, NULL, 0)) {
        return false;
    }

    i2c_initiator_start_condition();

    if (!i2c_initiator_write_byte((slave_address << 1) | (1 << 0))) {
        return false;
    }

    if (data != NULL) {
        for (size_t i = 0; i < data_len; i++) {
            data[i] = i2c_initiator_read_byte(data_len - i == 1);
        }
    }

    i2c_initiator_stop_condition();

    return true;
}

#endif
