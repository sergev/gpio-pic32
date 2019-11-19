/*
 * Command-line interface to a GPIO from PIC32 Linux.
 * Inspired by gpio utility from wiringPi by Gordon Henderson.
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
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <getopt.h>
#include "gpio.h"

const char version[] = "0.1";
const char copyright[] = "Copyright (C) 2019 Serge Vakulenko";

//
// Get a pic32 pin name by physical pin index at GPIO extension connector.
//
const char *phys_name[1+40] = {
    [0]  = "??",
    [1]  = "+3V3",  [2]  = "+5V",
    [3]  = "RF2",   [4]  = "+5V",
    [5]  = "RF8",   [6]  = "Gnd",
    [7]  = "RE4",   [8]  = "RC3",
    [9]  = "Gnd",   [10] = "RE8",
    [11] = "RE7",   [12] = "RH3",
    [13] = "RB8",   [14] = "Gnd",
    [15] = "RA9",   [16] = "RB4",
    [17] = "+3V3",  [18] = "RH4",
    [19] = "RG8",   [20] = "Gnd",
    [21] = "RD7",   [22] = "RH6",
    [23] = "RG6",   [24] = "RD0",
    [25] = "Gnd",   [26] = "RD14",
    [27] = "RB2",   [28] = "---",
    [29] = "RK1",   [30] = "Gnd",
    [31] = "RK2",   [32] = "RJ2",
    [33] = "RG9",   [34] = "Gnd",
    [35] = "RB0",   [36] = "RB15",
    [37] = "RH7",   [38] = "RH12",
    [39] = "Gnd",   [40] = "RD15",
};

//
// Get mode name by gpio_mode_t value.
//
const char *mode_name[] = {
    [MODE_OUTPUT]   = "Out",
    [MODE_INPUT]    = "In",
    [MODE_ANALOG]   = "Analog",
    [MODE_C1OUT]    = "C1OUT",
    [MODE_C1TX]     = "C1TX",
    [MODE_C2OUT]    = "C2OUT",
    [MODE_C2TX]     = "C2TX",
    [MODE_OC1]      = "OC1",
    [MODE_OC2]      = "OC2",
    [MODE_OC3]      = "OC3",
    [MODE_OC4]      = "OC4",
    [MODE_OC5]      = "OC5",
    [MODE_OC6]      = "OC6",
    [MODE_OC7]      = "OC7",
    [MODE_OC8]      = "OC8",
    [MODE_OC9]      = "OC9",
    [MODE_REFCLKO1] = "REFCLKO1",
    [MODE_REFCLKO3] = "REFCLKO3",
    [MODE_REFCLKO4] = "REFCLKO4",
    [MODE_SDO1]     = "SDO1",
    [MODE_SDO2]     = "SDO2",
    [MODE_SDO3]     = "SDO3",
    [MODE_SDO4]     = "SDO4",
    [MODE_SDO5]     = "SDO5",
    [MODE_SDO6]     = "SDO6",
    [MODE_SS1O]     = "SS1O",
    [MODE_SS2O]     = "SS2O",
    [MODE_SS3O]     = "SS3O",
    [MODE_SS4O]     = "SS4O",
    [MODE_SS5O]     = "SS5O",
    [MODE_SS6O]     = "SS6O",
    [MODE_U1RTS]    = "U1RTS",
    [MODE_U1TX]     = "U1TX",
    [MODE_U2RTS]    = "U2RTS",
    [MODE_U2TX]     = "U2TX",
    [MODE_U3RTS]    = "U3RTS",
    [MODE_U3TX]     = "U3TX",
    [MODE_U4RTS]    = "U4RTS",
    [MODE_U4TX]     = "U4TX",
    [MODE_U5RTS]    = "U5RTS",
    [MODE_U5TX]     = "U5TX",
    [MODE_U6RTS]    = "U6RTS",
    [MODE_U6TX]     = "U6TX",
    [MODE_C1RX]     = "C1RX",
    [MODE_C2RX]     = "C2RX",
    [MODE_IC1]      = "IC1",
    [MODE_IC2]      = "IC2",
    [MODE_IC3]      = "IC3",
    [MODE_IC4]      = "IC4",
    [MODE_IC5]      = "IC5",
    [MODE_IC6]      = "IC6",
    [MODE_IC7]      = "IC7",
    [MODE_IC8]      = "IC8",
    [MODE_IC9]      = "IC9",
    [MODE_INT1]     = "INT1",
    [MODE_INT2]     = "INT2",
    [MODE_INT3]     = "INT3",
    [MODE_INT4]     = "INT4",
    [MODE_OCFA]     = "OCFA",
    [MODE_REFCLKI1] = "REFCLKI1",
    [MODE_REFCLKI3] = "REFCLKI3",
    [MODE_REFCLKI4] = "REFCLKI4",
    [MODE_SDI1]     = "SDI1",
    [MODE_SDI2]     = "SDI2",
    [MODE_SDI3]     = "SDI3",
    [MODE_SDI4]     = "SDI4",
    [MODE_SDI5]     = "SDI5",
    [MODE_SDI6]     = "SDI6",
    [MODE_SS1I]     = "SS1I",
    [MODE_SS2I]     = "SS2I",
    [MODE_SS3I]     = "SS3I",
    [MODE_SS4I]     = "SS4I",
    [MODE_SS5I]     = "SS5I",
    [MODE_SS6I]     = "SS6I",
    [MODE_T2CK]     = "T2CK",
    [MODE_T3CK]     = "T3CK",
    [MODE_T4CK]     = "T4CK",
    [MODE_T5CK]     = "T5CK",
    [MODE_T6CK]     = "T6CK",
    [MODE_T7CK]     = "T7CK",
    [MODE_T8CK]     = "T8CK",
    [MODE_T9CK]     = "T9CK",
    [MODE_U1CTS]    = "U1CTS",
    [MODE_U1RX]     = "U1RX",
    [MODE_U2CTS]    = "U2CTS",
    [MODE_U2RX]     = "U2RX",
    [MODE_U3CTS]    = "U3CTS",
    [MODE_U3RX]     = "U3RX",
    [MODE_U4CTS]    = "U4CTS",
    [MODE_U4RX]     = "U4RX",
    [MODE_U5CTS]    = "U5CTS",
    [MODE_U5RX]     = "U5RX",
    [MODE_U6CTS]    = "U6CTS",
    [MODE_U6RX]     = "U6RX",
    [MODE_SCK1]     = "SCK1",
    [MODE_SCK2]     = "SCK2",
    [MODE_SCK3]     = "SCK3",
    [MODE_SCK4]     = "SCK4",
    [MODE_SCK5]     = "SCK5",
    [MODE_SCK6]     = "SCK6",
    [MODE_SCL1]     = "SCL1",
    [MODE_SCL2]     = "SCL2",
    [MODE_SCL3]     = "SCL3",
    [MODE_SCL4]     = "SCL4",
    [MODE_SCL5]     = "SCL5",
    [MODE_SCL6]     = "SCL6",
    [MODE_SDA1]     = "SDA1",
    [MODE_SDA2]     = "SDA2",
    [MODE_SDA3]     = "SDA3",
    [MODE_SDA4]     = "SDA4",
    [MODE_SDA5]     = "SDA5",
    [MODE_SDA6]     = "SDA6",
};

//
// Convert physical pin index at GPIO extension connector into a Broadcom index.
//
int phys_to_bcm(int phys)
{
    static int map[64] = {
        -1,         // j0
        -1, -1,     // j1,  j2
        2,  -1,     // j3,  j4
        3,  -1,     // j5,  j6
        4,  14,     // j7,  j8
        -1, 15,     // j9,  j10
        17, 18,     // j11, j12
        27, -1,     // j13, j14
        22, 23,     // j15, j16
        -1, 24,     // j17, j18
        10, -1,     // j19, j20
        9,  25,     // j21, j22
        11, 8,      // j23, j24
        -1, 7,      // j25, j26
        0,  -1,     // j27, j28 = p1 not connected
        5,  -1,     // j29, j30
        6,  12,     // j31, j32
        13, -1,     // j33, j34
        19, 16,     // j35, j36
        26, 20,     // j37, j38
        -1, 21,     // j39, j40
    };
    return map[phys & 63];
}

//
// Convert Broadcom index into a physical pin index at GPIO extension connector.
//
int bcm_to_phys(int bcm)
{
    static int map[32] = {
        27,         // p0
        -1,         // p1
        3,          // p2
        5,          // p3
        7,          // p4
        29,         // p5
        31,         // p6
        26,         // p7
        24,         // p8
        21,         // p9
        19,         // p10
        23,         // p11
        32,         // p12
        33,         // p13
        8,          // p14
        10,         // p15
        36,         // p16
        11,         // p17
        12,         // p18
        35,         // p19
        38,         // p20
        40,         // p21
        15,         // p22
        16,         // p23
        18,         // p24
        22,         // p25
        37,         // p26
        13,         // p27
        -1,         // 28
        -1,         // 29
        -1,         // 30
        -1,         // 31
    };
    return map[bcm & 31];
}

//
// Convert physical pin index at GPIO extension connector into a pin descriptor.
//
int phys_to_pin(int phys)
{
    switch (phys) {
    case 3:  return GPIO_PIN('F', 2);
    case 5:  return GPIO_PIN('F', 8);
    case 7:  return GPIO_PIN('E', 4);
    case 8:  return GPIO_PIN('C', 3);
    case 10: return GPIO_PIN('E', 8);
    case 11: return GPIO_PIN('E', 7);
    case 12: return GPIO_PIN('H', 3);
    case 13: return GPIO_PIN('B', 8);
    case 15: return GPIO_PIN('A', 9);
    case 16: return GPIO_PIN('B', 4);
    case 18: return GPIO_PIN('H', 4);
    case 19: return GPIO_PIN('G', 8);
    case 21: return GPIO_PIN('D', 7);
    case 22: return GPIO_PIN('H', 6);
    case 23: return GPIO_PIN('G', 6);
    case 24: return GPIO_PIN('D', 0);
    case 26: return GPIO_PIN('D', 14);
    case 27: return GPIO_PIN('B', 2);
    case 29: return GPIO_PIN('K', 1);
    case 31: return GPIO_PIN('K', 2);
    case 32: return GPIO_PIN('J', 2);
    case 33: return GPIO_PIN('G', 9);
    case 35: return GPIO_PIN('B', 0);
    case 36: return GPIO_PIN('B', 15);
    case 37: return GPIO_PIN('H', 7);
    case 38: return GPIO_PIN('H', 12);
    case 40: return GPIO_PIN('D', 15);
    default: return -1;
    };
}

//
// Get a pin descriptor by a pic32 pin name.
//
int pin_by_name(const char *name)
{
    if (name[0] == 'r' || name[0] == 'R') {
        // PIC32 pin names.
        if      (strcasecmp(name, "RA9")  == 0) return GPIO_PIN('A', 9);
        else if (strcasecmp(name, "RB0")  == 0) return GPIO_PIN('B', 0);
        else if (strcasecmp(name, "RB2")  == 0) return GPIO_PIN('B', 2);
        else if (strcasecmp(name, "RB4")  == 0) return GPIO_PIN('B', 4);
        else if (strcasecmp(name, "RB8")  == 0) return GPIO_PIN('B', 8);
        else if (strcasecmp(name, "RB15") == 0) return GPIO_PIN('B', 15);
        else if (strcasecmp(name, "RC3")  == 0) return GPIO_PIN('C', 3);
        else if (strcasecmp(name, "RD0")  == 0) return GPIO_PIN('D', 0);
        else if (strcasecmp(name, "RD7")  == 0) return GPIO_PIN('D', 7);
        else if (strcasecmp(name, "RD14") == 0) return GPIO_PIN('D', 14);
        else if (strcasecmp(name, "RD15") == 0) return GPIO_PIN('D', 15);
        else if (strcasecmp(name, "RE4")  == 0) return GPIO_PIN('E', 4);
        else if (strcasecmp(name, "RE7")  == 0) return GPIO_PIN('E', 7);
        else if (strcasecmp(name, "RE8")  == 0) return GPIO_PIN('E', 8);
        else if (strcasecmp(name, "RF2")  == 0) return GPIO_PIN('F', 2);
        else if (strcasecmp(name, "RF8")  == 0) return GPIO_PIN('F', 8);
        else if (strcasecmp(name, "RG6")  == 0) return GPIO_PIN('G', 6);
        else if (strcasecmp(name, "RG8")  == 0) return GPIO_PIN('G', 8);
        else if (strcasecmp(name, "RG9")  == 0) return GPIO_PIN('G', 9);
        else if (strcasecmp(name, "RH3")  == 0) return GPIO_PIN('H', 3);
        else if (strcasecmp(name, "RH4")  == 0) return GPIO_PIN('H', 4);
        else if (strcasecmp(name, "RH6")  == 0) return GPIO_PIN('H', 6);
        else if (strcasecmp(name, "RH7")  == 0) return GPIO_PIN('H', 7);
        else if (strcasecmp(name, "RH12") == 0) return GPIO_PIN('H', 12);
        else if (strcasecmp(name, "RJ2")  == 0) return GPIO_PIN('J', 2);
        else if (strcasecmp(name, "RK1")  == 0) return GPIO_PIN('K', 1);
        else if (strcasecmp(name, "RK2")  == 0) return GPIO_PIN('K', 2);
    } else
    if (name[0] == 'j' || name[0] == 'J') {
        // Physical pin indices on Extension connector.
        if      (strcasecmp(name, "j3")  == 0) return GPIO_PIN('F', 2);
        else if (strcasecmp(name, "j5")  == 0) return GPIO_PIN('F', 8);
        else if (strcasecmp(name, "j7")  == 0) return GPIO_PIN('E', 4);
        else if (strcasecmp(name, "j8")  == 0) return GPIO_PIN('C', 3);
        else if (strcasecmp(name, "j10") == 0) return GPIO_PIN('E', 8);
        else if (strcasecmp(name, "j11") == 0) return GPIO_PIN('E', 7);
        else if (strcasecmp(name, "j12") == 0) return GPIO_PIN('H', 3);
        else if (strcasecmp(name, "j13") == 0) return GPIO_PIN('B', 8);
        else if (strcasecmp(name, "j15") == 0) return GPIO_PIN('A', 9);
        else if (strcasecmp(name, "j16") == 0) return GPIO_PIN('B', 4);
        else if (strcasecmp(name, "j18") == 0) return GPIO_PIN('H', 4);
        else if (strcasecmp(name, "j19") == 0) return GPIO_PIN('G', 8);
        else if (strcasecmp(name, "j21") == 0) return GPIO_PIN('D', 7);
        else if (strcasecmp(name, "j22") == 0) return GPIO_PIN('H', 6);
        else if (strcasecmp(name, "j23") == 0) return GPIO_PIN('G', 6);
        else if (strcasecmp(name, "j24") == 0) return GPIO_PIN('D', 0);
        else if (strcasecmp(name, "j26") == 0) return GPIO_PIN('D', 14);
        else if (strcasecmp(name, "j27") == 0) return GPIO_PIN('B', 2);
        else if (strcasecmp(name, "j29") == 0) return GPIO_PIN('K', 1);
        else if (strcasecmp(name, "j31") == 0) return GPIO_PIN('K', 2);
        else if (strcasecmp(name, "j32") == 0) return GPIO_PIN('J', 2);
        else if (strcasecmp(name, "j33") == 0) return GPIO_PIN('G', 9);
        else if (strcasecmp(name, "j35") == 0) return GPIO_PIN('B', 0);
        else if (strcasecmp(name, "j36") == 0) return GPIO_PIN('B', 15);
        else if (strcasecmp(name, "j37") == 0) return GPIO_PIN('H', 7);
        else if (strcasecmp(name, "j38") == 0) return GPIO_PIN('H', 12);
        else if (strcasecmp(name, "j40") == 0) return GPIO_PIN('D', 15);
    } else
    if (name[0] == 'p' || name[0] == 'P') {
        // Broadcom pin names.
        if      (strcasecmp(name, "p0")  == 0) return GPIO_PIN('B', 2);
        else if (strcasecmp(name, "p2")  == 0) return GPIO_PIN('F', 2);
        else if (strcasecmp(name, "p3")  == 0) return GPIO_PIN('F', 8);
        else if (strcasecmp(name, "p4")  == 0) return GPIO_PIN('E', 4);
        else if (strcasecmp(name, "p5")  == 0) return GPIO_PIN('K', 1);
        else if (strcasecmp(name, "p6")  == 0) return GPIO_PIN('K', 2);
        else if (strcasecmp(name, "p7")  == 0) return GPIO_PIN('D', 14);
        else if (strcasecmp(name, "p8")  == 0) return GPIO_PIN('D', 0);
        else if (strcasecmp(name, "p9")  == 0) return GPIO_PIN('D', 7);
        else if (strcasecmp(name, "p10") == 0) return GPIO_PIN('G', 8);
        else if (strcasecmp(name, "p11") == 0) return GPIO_PIN('G', 6);
        else if (strcasecmp(name, "p12") == 0) return GPIO_PIN('J', 2);
        else if (strcasecmp(name, "p13") == 0) return GPIO_PIN('G', 9);
        else if (strcasecmp(name, "p14") == 0) return GPIO_PIN('C', 3);
        else if (strcasecmp(name, "p15") == 0) return GPIO_PIN('E', 8);
        else if (strcasecmp(name, "p16") == 0) return GPIO_PIN('B', 15);
        else if (strcasecmp(name, "p17") == 0) return GPIO_PIN('E', 7);
        else if (strcasecmp(name, "p18") == 0) return GPIO_PIN('H', 3);
        else if (strcasecmp(name, "p19") == 0) return GPIO_PIN('B', 0);
        else if (strcasecmp(name, "p20") == 0) return GPIO_PIN('H', 12);
        else if (strcasecmp(name, "p21") == 0) return GPIO_PIN('D', 15);
        else if (strcasecmp(name, "p22") == 0) return GPIO_PIN('A', 9);
        else if (strcasecmp(name, "p23") == 0) return GPIO_PIN('B', 4);
        else if (strcasecmp(name, "p24") == 0) return GPIO_PIN('H', 4);
        else if (strcasecmp(name, "p25") == 0) return GPIO_PIN('H', 6);
        else if (strcasecmp(name, "p26") == 0) return GPIO_PIN('H', 7);
        else if (strcasecmp(name, "p27") == 0) return GPIO_PIN('B', 8);
        else if (strcasecmp(name, "p1") == 0) {
            fprintf(stderr, "gpio: Pin name P1 is not supported on PIC32.\n");
            exit(-1);
        }
    }
    fprintf(stderr, "gpio: Wrong pin name: %s\n", name);
    fprintf(stderr, "gpio: Valid names are ra9-rk2, p0-p27, j3-j40\n");
    exit(-1);
    return 0;
}

//
// Print usage message.
//
void usage()
{
    fprintf(stderr, "GPIO control for PIC32, Version %s, %s\n", version, copyright);
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    gpio mode <pin> <mode>\n");
    fprintf(stderr, "    gpio read <pin>\n");
    fprintf(stderr, "    gpio write <pin> <value>\n");
    fprintf(stderr, "    gpio toggle <pin>\n");
    fprintf(stderr, "    gpio blink <pin>\n");
    fprintf(stderr, "    gpio readall\n");
    fprintf(stderr, "    gpio modes\n");
    fprintf(stderr, "Pins:\n");
    fprintf(stderr, "    ra9...rk2      PIC32 pin names\n");
    fprintf(stderr, "    p0...p27       Broadcom pin names\n");
    fprintf(stderr, "    j3...j40       Physical pins on the 40-pin header\n");
    fprintf(stderr, "Modes:\n");
    fprintf(stderr, "    in, input      Input\n");
    fprintf(stderr, "    out, output    Output\n");
    fprintf(stderr, "    up             Pull-up resistor (input only)\n");
    fprintf(stderr, "    down           Pull-down resistor (input only)\n");
    fprintf(stderr, "    tri, off       No pull-up/down resistor\n");
}

//
// Find mode by name.
//
static gpio_mode_t find_mode(const char *name)
{
    gpio_mode_t mode;

    for (mode=0; mode<MODE_LAST; mode++) {
        if (strcasecmp(name, mode_name[mode]) == 0)
            return mode;
    }
    fprintf(stderr, "gpio: Invalid mode: %s\n", name);
    exit(-1);
}

//
// gpio mode <pin> <mode>
//
void do_mode(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: gpio mode <pin> <mode>\n");
        exit(-1);
    }

    int pin = pin_by_name(argv[1]);
    const char *mode = argv[2];

    if      (strcasecmp(mode, "in")     == 0) gpio_set_mode(pin, MODE_INPUT);
    else if (strcasecmp(mode, "input")  == 0) gpio_set_mode(pin, MODE_INPUT);
    else if (strcasecmp(mode, "out")    == 0) gpio_set_mode(pin, MODE_OUTPUT);
    else if (strcasecmp(mode, "output") == 0) gpio_set_mode(pin, MODE_OUTPUT);
    else if (strcasecmp(mode, "up")     == 0) gpio_set_pull(pin, PULL_UP);
    else if (strcasecmp(mode, "down")   == 0) gpio_set_pull(pin, PULL_DOWN);
    else if (strcasecmp(mode, "tri")    == 0) gpio_set_pull(pin, PULL_OFF);
    else if (strcasecmp(mode, "off")    == 0) gpio_set_pull(pin, PULL_OFF);
    else                                      gpio_set_mode(pin, find_mode(mode));
}

//
// gpio read <pin>
//
void do_read(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: gpio read <pin>\n");
        exit(-1);
    }

    int pin = pin_by_name(argv[1]);
    int val = gpio_read(pin);

    printf("%s\n", val == 0 ? "0" : "1");
}

//
// gpio write <pin> <value>
//
void do_write(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: gpio write <pin> <value>\n");
        exit(-1);
    }

    int pin = pin_by_name(argv[1]);

    int val;
    if      (strcasecmp(argv[2], "up")   == 0) val = 1;
    else if (strcasecmp(argv[2], "on")   == 0) val = 1;
    else if (strcasecmp(argv[2], "down") == 0) val = 0;
    else if (strcasecmp(argv[2], "off")  == 0) val = 0;
    else {
        val = strtol(argv[2], 0, 0);
    }

    if (val == 0) {
        gpio_write(pin, 0);
    } else {
        gpio_write(pin, 1);
    }
}

//
// gpio toggle <pin>
//
void do_toggle(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: gpio toggle <pin>\n");
        exit(-1);
    }

    int pin = pin_by_name(argv[1]);

    gpio_toggle(pin);
}

//
// gpio blink <pin>
//
void do_blink(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: gpio blink <pin>\n");
        exit(-1);
    }

    int pin = pin_by_name(argv[1]);

    gpio_set_mode(pin, MODE_OUTPUT);
    for (;;) {
        gpio_toggle(pin);
        usleep(500000);
    }
}

//
// Print status of all pins on GPIO extension connector.
//
void do_readall()
{
    printf(" +-----+------+--------+---+------------+---+--------+------+-----+\n");
    printf(" | BCM | Name | Mode   | V |  Physical  | V | Mode   | Name | BCM |\n");
    printf(" +-----+------+--------+---+-----++-----+---+--------+------+-----+\n");

    int phys;
    for (phys = 1; phys <= 40; phys += 2) {
        int bcm = phys_to_bcm(phys);
        if (bcm < 0) {
            printf(" |     | %-4s |        |  ", phys_name[phys]);
        } else {
            int pin = phys_to_pin(phys);
            int mode = gpio_get_mode(pin);

            printf(" | p%-2d", bcm);
            printf(" | %-4s", phys_name[phys]);
            printf(" | %-6s", mode_name[mode]);
            if (mode == MODE_ANALOG)
                printf(" | -");
            else
                printf(" | %d", gpio_read(pin));
        }

        // Pin numbers
        printf(" | j%-2d || j%-2d", phys, phys+1);

        // Same, reversed
        bcm = phys_to_bcm(phys+1);
        if (bcm < 0) {
            printf(" |   |        | %-4s |    ", phys_name[phys+1]);
        } else {
            int pin = phys_to_pin(phys+1);
            int mode = gpio_get_mode(pin);

            if (mode == MODE_ANALOG)
                printf(" | -");
            else
                printf(" | %d", gpio_read(pin));
            printf(" | %-6s", mode_name[mode]);
            printf(" | %-4s", phys_name[phys+1]);
            printf(" | p%-2d", bcm);
        }
        printf(" |\n");
    }
    printf(" +-----+------+--------+---+-----++-----+---+--------+------+-----+\n");
    printf(" | BCM | Name | Mode   | V |  Physical  | V | Mode   | Name | BCM |\n");
    printf(" +-----+------+--------+---+------------+---+--------+------+-----+\n");
}

//
// For every mode, show available pins.
//
void do_modes()
{
    printf(" Mode     Available Pins\n");

    gpio_mode_t mode;
    for (mode=MODE_ANALOG+1; mode<MODE_LAST; mode++) {
        printf(" %-8s", mode_name[mode]);

        // Print pins, capable of this mode.
        int phys;
        for (phys = 1; phys <= 40; phys++) {
            int bcm = phys_to_bcm(phys);
            if (bcm < 0)
                continue;

            int pin = phys_to_pin(phys);
            if (gpio_has_mapping(pin, mode)) {
                //printf(" j%d ", phys);
                printf(" p%d", bcm);
                //printf(" %s", phys_name[phys]);
            }
        }
        printf("\n");
    }
}

//
// For every pin, show possible modes.
//
void do_pins()
{
    printf(" Pin Phys Name Available Modes\n");
    int bcm;
    for (bcm = 0; bcm < 32; bcm++) {
        int phys = bcm_to_phys(bcm);
        if (phys < 0)
            continue;

        // Print alternative mappings, available for this pin.
        int pin = phys_to_pin(phys);
        int print_this_pin = 0;
        gpio_mode_t mode;

        // First line: output modes.
        for (mode=MODE_ANALOG+1; mode<MODE_C1RX; mode++) {
            if (gpio_has_mapping(pin, mode)) {
                if (print_this_pin == 0) {
                    print_this_pin = 1;
                    printf(" p%-2d", bcm);
                    printf(" j%-2d ", phys);
                    printf(" %-4s", phys_name[phys]);
                }
                printf(" %s", mode_name[mode]);
            }
        }
        if (print_this_pin) {
            // Second line: input modes.
            printf("\n              ");
            for (mode=MODE_C1RX; mode<MODE_LAST; mode++) {
                if (gpio_has_mapping(pin, mode)) {
                    printf(" %s", mode_name[mode]);
                }
            }
            printf("\n");
        }
    }
}

int main(int argc, char **argv)
{
    const char *env_debug = getenv("GPIO_DEBUG");

    for (;;) {
        switch (getopt(argc, argv, "vhd")) {
        case EOF:
            break;
        case 'v':
            printf("gpio version: %s\n", version);
            return 0;
        case 'h':
            usage();
            return 0;
        case 'd':
            ++gpio_debug;
            continue;
        default:
            usage();
        }
        break;
    }
    argc -= optind;
    argv += optind;

    if (argc < 1) {
        usage();
        return -1;
    }

    if (!gpio_debug && env_debug)
        gpio_debug = atoi(env_debug);

    if (strcasecmp(argv[0], "pins") == 0) {
        do_pins();
        return 0;
    }
    if (strcasecmp(argv[0], "modes") == 0) {
        do_modes();
        return 0;
    }

    if (geteuid() != 0) {
        fprintf(stderr, "gpio: Must be root to run.\n");
        return -1;
    }

    if      (strcasecmp(argv[0], "mode")    == 0) do_mode(argc, argv);
    else if (strcasecmp(argv[0], "read")    == 0) do_read(argc, argv);
    else if (strcasecmp(argv[0], "write")   == 0) do_write(argc, argv);
    else if (strcasecmp(argv[0], "toggle")  == 0) do_toggle(argc, argv);
    else if (strcasecmp(argv[0], "blink")   == 0) do_blink(argc, argv);
    else if (strcasecmp(argv[0], "readall") == 0) do_readall();
    else {
        fprintf(stderr, "gpio: Unknown command: %s.\n", argv[0]);
        return -1;
    }

    return 0;
}
