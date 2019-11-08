/*
 * GPIO interface for PIC32.
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

/*
 * Pin modes.
 */
typedef enum {
    MODE_OUTPUT,            /* Output */
    MODE_INPUT,             /* Input */
    MODE_ANALOG,            /* Analog */

    // Alternative functions: outputs
    MODE_C1OUT,
    MODE_C1TX,
    MODE_C2OUT,
    MODE_C2TX,
    MODE_OC1,
    MODE_OC2,
    MODE_OC3,
    MODE_OC4,
    MODE_OC5,
    MODE_OC6,
    MODE_OC7,
    MODE_OC8,
    MODE_OC9,
    MODE_REFCLKO1,
    MODE_REFCLKO3,
    MODE_REFCLKO4,
    MODE_SDO1,
    MODE_SDO2,
    MODE_SDO3,
    MODE_SDO4,
    MODE_SDO5,
    MODE_SDO6,
    MODE_SS1,
    MODE_SS2,
    MODE_SS3,
    MODE_SS4,
    MODE_SS5,
    MODE_SS6,
    MODE_U1RTS,
    MODE_U1TX,
    MODE_U2RTS,
    MODE_U2TX,
    MODE_U3RTS,
    MODE_U3TX,
    MODE_U4RTS,
    MODE_U4TX,
    MODE_U5RTS,
    MODE_U5TX,
    MODE_U6RTS,
    MODE_U6TX,

    // Alternative functions: inputs
    //TODO: alternative input modes
} gpio_mode_t;

/*
 * Pull up/down modes.
 */
typedef enum {
    PULL_OFF    = 0,        /* No pull up/down */
    PULL_UP     = 1,        /* Pull up */
    PULL_DOWN   = 2,        /* Pull down */
} gpio_pull_t;

/*
 * Set pin direction or alternative function.
 */
int gpio_set_mode(int pin, gpio_mode_t dir);

/*
 * Get pin direction or alternative function.
 */
gpio_mode_t gpio_get_mode(int pin);

/*
 * Set pull up/down resistors.
 */
int gpio_set_pull(int pin, gpio_pull_t pull);

/*
 * Read the input value. This can be 0 or 1.
 * Return -1 in case of error.
 */
int gpio_read(int pin);

/*
 * Write the output value.
 */
int gpio_write(int pin, int value);

/*
 * Toggle the output value.
 */
int gpio_toggle(int pin);

/*
 * Calculate register offset by port name.
 */
#define GPIO_OFFSET(port) (port == 'A' ? 0x000 : \
                           port == 'B' ? 0x100 : \
                           port == 'C' ? 0x200 : \
                           port == 'D' ? 0x300 : \
                           port == 'E' ? 0x400 : \
                           port == 'F' ? 0x500 : \
                           port == 'G' ? 0x600 : \
                           port == 'H' ? 0x700 : \
                           port == 'J' ? 0x800 : 0x900)
#define GPIO_PIN(port, bitnum) ((GPIO_OFFSET(port) << 16) | (1 << bitnum))

gpio_mode_t gpio_get_output_mapping(int pin);
gpio_mode_t gpio_get_input_mapping(int pin);
