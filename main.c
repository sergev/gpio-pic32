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

const char *dir_name[] = {
    [MODE_OUTPUT] = "Out",
    [MODE_INPUT]  = "In",
    [MODE_ANALOG] = "Analog",
};

int phys_to_bcm(int phys)
{
    static int map[64] = {
        -1,         // 0
        -1, -1,     // 1,  2
        2,  -1,     // 3,  4
        3,  -1,     // 5,  6
        4,  14,     // 7,  8
        -1, 15,     // 9,  10
        17, 18,     // 11, 12
        27, -1,     // 13, 14
        22, 23,     // 15, 16
        -1, 24,     // 17, 18
        10, -1,     // 19, 20
        9,  25,     // 21, 22
        11, 8,      // 23, 24
        -1, 7,      // 25, 26
        0,  -1,     // 27, 28 bcm1 not connected
        5,  -1,     // 29, 30
        6,  12,     // 31, 32
        13, -1,     // 33, 34
        19, 16,     // 35, 36
        26, 20,     // 37, 38
        -1, 21,     // 39, 40
    };
    return map[phys & 63];
}

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

int debug;

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

void do_mode(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: gpio mode <pin> <mode>\n");
        exit(-1);
    }

    int pin = pin_by_name(argv[1]);
    const char *mode = argv[2];

    if      (strcasecmp(mode, "in")     == 0) gpio_set_dir(pin, MODE_INPUT);
    else if (strcasecmp(mode, "input")  == 0) gpio_set_dir(pin, MODE_INPUT);
    else if (strcasecmp(mode, "out")    == 0) gpio_set_dir(pin, MODE_OUTPUT);
    else if (strcasecmp(mode, "output") == 0) gpio_set_dir(pin, MODE_OUTPUT);
    else if (strcasecmp(mode, "up")     == 0) gpio_set_pull(pin, PULL_UP);
    else if (strcasecmp(mode, "down")   == 0) gpio_set_pull(pin, PULL_DOWN);
    else if (strcasecmp(mode, "tri")    == 0) gpio_set_pull(pin, PULL_OFF);
    else if (strcasecmp(mode, "off")    == 0) gpio_set_pull(pin, PULL_OFF);
    else {
        fprintf(stderr, "gpio: Invalid mode: %s\n", mode);
        exit(-1);
    }
}

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

void do_toggle(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: gpio toggle <pin>\n");
        exit(-1);
    }

    int pin = pin_by_name(argv[1]);

    gpio_toggle(pin);
}

void do_blink(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: gpio blink <pin>\n");
        exit(-1);
    }

    int pin = pin_by_name(argv[1]);

    gpio_set_dir(pin, MODE_OUTPUT);
    for (;;) {
        gpio_toggle(pin);
        usleep(500000);
    }
}

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
            int dir = gpio_get_dir(pin);

            printf(" | p%-2d", bcm);
            printf(" | %-4s", phys_name[phys]);
            printf(" | %-6s", dir_name[dir]);
            if (dir == MODE_ANALOG)
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
            int dir = gpio_get_dir(pin);

            if (dir == MODE_ANALOG)
                printf(" | -");
            else
                printf(" | %d", gpio_read(pin));
            printf(" | %-6s", dir_name[dir]);
            printf(" | %-4s", phys_name[phys+1]);
            printf(" | p%-2d", bcm);
        }
        printf(" |\n");
    }
    printf(" +-----+------+--------+---+-----++-----+---+--------+------+-----+\n");
    printf(" | BCM | Name | Mode   | V |  Physical  | V | Mode   | Name | BCM |\n");
    printf(" +-----+------+--------+---+------------+---+--------+------+-----+\n");
}

int main(int argc, char **argv)
{
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
            ++debug;
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

    if      (strcasecmp(argv[0], "mode")    == 0) do_mode(argc, argv);
    else if (strcasecmp(argv[0], "read")    == 0) do_read(argc, argv);
    else if (strcasecmp(argv[0], "write")   == 0) do_write(argc, argv);
    else if (strcasecmp(argv[0], "toggle")  == 0) do_toggle(argc, argv);
    else if (strcasecmp(argv[0], "blink")   == 0) do_blink(argc, argv);
    else if (strcasecmp(argv[0], "readall") == 0) do_readall();
    else if (strcasecmp(argv[0], "pins")    == 0) do_readall();
    else {
        fprintf(stderr, "gpio: Unknown command: %s.\n", argv[0]);
        return -1;
    }

    return 0;
}
