/*
 * I2C functions.
 *
 * Copyright (C) 2019 Serge Vakulenko
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/mman.h>
#include "gpio.h"

//
// I2C control registers
//
#define I2C1CON 0x0000
#define I2C2CON 0x0200
#define I2C3CON 0x0400
#define I2C4CON 0x0600
#define I2C5CON 0x0800

static const int I2C_ADDR = 0x1f820000;

static ptrdiff_t i2c_base;          // PPS registers mapped here

//
// Get access to PPS control registers.
// Set i2c_base to a base address of the appropriate page.
//
static void i2c_init()
{
    extern int gpio_mem_fd;

    // Map a page of memory to the PPS address
    i2c_base = (ptrdiff_t) mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED,
        gpio_mem_fd, I2C_ADDR);
    if (i2c_base < 0) {
        printf("PPS mmap failed: %s\n", strerror(errno));
        exit(-1);
    }
}

//
// Get output mapping for a given pin.
//
gpio_mode_t gpio_get_i2c_function(int pin)
{
    int offset;
    gpio_mode_t mode;

    switch (pin) {
    default:
        return 0;

    case GPIO_PIN('A',14): offset = I2C1CON; mode = MODE_SCL1; break;
    case GPIO_PIN('A',2):  offset = I2C2CON; mode = MODE_SCL2; break;
    case GPIO_PIN('F',8):  offset = I2C3CON; mode = MODE_SCL3; break;
    case GPIO_PIN('G',8):  offset = I2C4CON; mode = MODE_SCL4; break;
    case GPIO_PIN('F',5):  offset = I2C5CON; mode = MODE_SCL5; break;

    case GPIO_PIN('A',15): offset = I2C1CON; mode = MODE_SDA1; break;
    case GPIO_PIN('A',3):  offset = I2C2CON; mode = MODE_SDA2; break;
    case GPIO_PIN('F',2):  offset = I2C3CON; mode = MODE_SDA3; break;
    case GPIO_PIN('G',7):  offset = I2C4CON; mode = MODE_SDA4; break;
    case GPIO_PIN('F',4):  offset = I2C5CON; mode = MODE_SDA5; break;
    }

    if (!i2c_base)
        i2c_init();

    // Read I2CxCON register.
    volatile uint32_t *regp = (uint32_t*) (i2c_base + offset);
    uint32_t spicon = *regp;
    if (gpio_debug > 1)
        printf("--- %s: [%08x] -> %08x\n", __func__, I2C_ADDR + offset, spicon);

    // Check ON bit.
    if (spicon & 0x00008000) {
        // I2C Peripheral is enabled.
        return mode;
    }
    return 0;
}
