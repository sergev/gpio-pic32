/*
 * SPI functions.
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
// SPI control registers
//
#define SPI1CON 0x0000
#define SPI2CON 0x0200
#define SPI3CON 0x0400
#define SPI4CON 0x0600
#define SPI5CON 0x0800
#define SPI6CON 0x0a00

static const int SPI_ADDR = 0x1f821000;

static ptrdiff_t spi_base;          // PPS registers mapped here

//
// Get access to PPS control registers.
// Set spi_base to a base address of the appropriate page.
//
static void spi_init()
{
    extern int gpio_mem_fd;

    // Map a page of memory to the PPS address
    spi_base = (ptrdiff_t) mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED,
        gpio_mem_fd, SPI_ADDR);
    if (spi_base < 0) {
        printf("PPS mmap failed: %s\n", strerror(errno));
        exit(-1);
    }
}

//
// Get output mapping for a given pin.
//
gpio_mode_t gpio_get_spi_function(int pin)
{
    int offset;
    gpio_mode_t mode;

    switch (pin) {
    default:
        return 0;
    case GPIO_PIN('D',1):  offset = SPI1CON; mode = MODE_SCK1; break;
    case GPIO_PIN('G',6):  offset = SPI2CON; mode = MODE_SCK2; break;
    case GPIO_PIN('B',14): offset = SPI3CON; mode = MODE_SCK3; break;
    case GPIO_PIN('D',10): offset = SPI4CON; mode = MODE_SCK4; break;
    case GPIO_PIN('F',13): offset = SPI5CON; mode = MODE_SCK5; break;
    case GPIO_PIN('D',15): offset = SPI6CON; mode = MODE_SCK6; break;
    }

    if (!spi_base)
        spi_init();

    // Read SPICON register.
    volatile uint32_t *regp = (uint32_t*) (spi_base + offset);
    uint32_t spicon = *regp;
    if (gpio_debug > 1)
        printf("--- %s: [%08x] -> %08x\n", __func__, SPI_ADDR + offset, spicon);

    // Check ON bit.
    if (spicon & 0x00008000) {
        // SPI Peripheral is enabled.
        return mode;
    }
    return 0;
}
