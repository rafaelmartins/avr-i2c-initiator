/*
 * avr-i2c-initiator: An implementation of I2C bit-bang initiator for AVR
 *                    microcontrollers.
 * Copyright (C) 2020 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "i2c-initiator.h"


int
main(void)
{
    i2c_initiator_init(A, 6, 4);
    uint8_t a;
    while (true) {
        for (uint8_t i = 0; i < 10; i++) {
            i2c_initiator_write(0x20, 0x00, &i, 1);
            _delay_ms(1000);
            if (i2c_initiator_read(0x20, 0x00, &a, 1)) {
                a = 9 - a;
                i2c_initiator_write(0x20, 0x00, &a, 1);
                _delay_ms(1000);
            }
        }
    }

    return 0;
}
