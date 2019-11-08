/*
 * Peripheral port select registers Microchip PIC32MZ microcontroller.
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
#include <stdint.h>
#include "gpio.h"

#define INT1R           0x1404
#define INT2R           0x1408
#define INT3R           0x140C
#define INT4R           0x1410
#define T2CKR           0x1418
#define T3CKR           0x141C
#define T4CKR           0x1420
#define T5CKR           0x1424
#define T6CKR           0x1428
#define T7CKR           0x142C
#define T8CKR           0x1430
#define T9CKR           0x1434
#define IC1R            0x1438
#define IC2R            0x143C
#define IC3R            0x1440
#define IC4R            0x1444
#define IC5R            0x1448
#define IC6R            0x144C
#define IC7R            0x1450
#define IC8R            0x1454
#define IC9R            0x1458
#define OCFAR           0x1460
#define U1RXR           0x1468
#define U1CTSR          0x146C
#define U2RXR           0x1470
#define U2CTSR          0x1474
#define U3RXR           0x1478
#define U3CTSR          0x147C
#define U4RXR           0x1480
#define U4CTSR          0x1484
#define U5RXR           0x1488
#define U5CTSR          0x148C
#define U6RXR           0x1490
#define U6CTSR          0x1494
#define SDI1R           0x149C
#define SS1R            0x14A0
#define SDI2R           0x14A8
#define SS2R            0x14AC
#define SDI3R           0x14B4
#define SS3R            0x14B8
#define SDI4R           0x14C0
#define SS4R            0x14C4
#define SDI5R           0x14CC
#define SS5R            0x14D0
#define SDI6R           0x14D8
#define SS6R            0x14DC
#define C1RXR           0x14E0
#define C2RXR           0x14E4
#define REFCLKI1R       0x14E8
#define REFCLKI3R       0x14F0
#define REFCLKI4R       0x14F4

#define RPA14R          0x1538
#define RPA15R          0x153C
#define RPB0R           0x1540
#define RPB1R           0x1544
#define RPB2R           0x1548
#define RPB3R           0x154C
#define RPB5R           0x1554
#define RPB6R           0x1558
#define RPB7R           0x155C
#define RPB8R           0x1560
#define RPB9R           0x1564
#define RPB10R          0x1568
#define RPB14R          0x1578
#define RPB15R          0x157C
#define RPC1R           0x1584
#define RPC2R           0x1588
#define RPC3R           0x158C
#define RPC4R           0x1590
#define RPC13R          0x15B4
#define RPC14R          0x15B8
#define RPD0R           0x15C0
#define RPD1R           0x15C4
#define RPD2R           0x15C8
#define RPD3R           0x15CC
#define RPD4R           0x15D0
#define RPD5R           0x15D4
#define RPD6R           0x15D8
#define RPD7R           0x15DC
#define RPD9R           0x15E4
#define RPD10R          0x15E8
#define RPD11R          0x15EC
#define RPD12R          0x15F0
#define RPD14R          0x15F8
#define RPD15R          0x15FC
#define RPE3R           0x160C
#define RPE5R           0x1614
#define RPE8R           0x1620
#define RPE9R           0x1624
#define RPF0R           0x1640
#define RPF1R           0x1644
#define RPF2R           0x1648
#define RPF3R           0x164C
#define RPF4R           0x1650
#define RPF5R           0x1654
#define RPF8R           0x1660
#define RPF12R          0x1670
#define RPF13R          0x1674
#define RPG0R           0x1680
#define RPG1R           0x1684
#define RPG6R           0x1698
#define RPG7R           0x169C
#define RPG8R           0x16A0
#define RPG9R           0x16A4

uint32_t read_sfr(int offset)
{
    //TODO: read SFRs
    return 0;
}

static gpio_mode_t mode_output_group1(uint32_t value)
{
    switch (value & 0x0f) {
    default:
    case 0:  return 0;          // No Connect
    case 1:  return MODE_U3TX;
    case 2:  return MODE_U4RTS;
    case 3:  return 0;          // Reserved
    case 4:  return 0;          // Reserved
    case 5:  return MODE_SDO1;
    case 6:  return MODE_SDO2;
    case 7:  return MODE_SDO3;
    case 8:  return 0;          // Reserved
    case 9:  return MODE_SDO5;
    case 10: return MODE_SS6;
    case 11: return MODE_OC3;
    case 12: return MODE_OC6;
    case 13: return MODE_REFCLKO4;
    case 14: return MODE_C2OUT;
    case 15: return MODE_C1TX;
    }
}

static gpio_mode_t mode_output_group2(uint32_t value)
{
    switch (value & 0x0f) {
    default:
    case 0:  return 0;          // No Connect
    case 1:  return MODE_U1TX;
    case 2:  return MODE_U2RTS;
    case 3:  return MODE_U5TX;
    case 4:  return MODE_U6RTS;
    case 5:  return MODE_SDO1;
    case 6:  return MODE_SDO2;
    case 7:  return MODE_SDO3;
    case 8:  return MODE_SDO4;
    case 9:  return MODE_SDO5;
    case 10: return 0;          // Reserved
    case 11: return MODE_OC4;
    case 12: return MODE_OC7;
    case 13: return 0;          // Reserved
    case 14: return 0;          // Reserved
    case 15: return MODE_REFCLKO1;
    }
}

static gpio_mode_t mode_output_group3(uint32_t value)
{
    switch (value & 0x0f) {
    default:
    case 0:  return 0;          // No Connect
    case 1:  return MODE_U3RTS;
    case 2:  return MODE_U4TX;
    case 3:  return 0;          // Reserved
    case 4:  return MODE_U6TX;
    case 5:  return MODE_SS1;
    case 6:  return 0;          // Reserved
    case 7:  return MODE_SS3;
    case 8:  return MODE_SS4;
    case 9:  return MODE_SS5;
    case 10: return MODE_SDO6;
    case 11: return MODE_OC5;
    case 12: return MODE_OC8;
    case 13: return 0;          // Reserved
    case 14: return MODE_C1OUT;
    case 15: return MODE_REFCLKO3;
    }
}

static gpio_mode_t mode_output_group4(uint32_t value)
{
    switch (value & 0x0f) {
    default:
    case 0:  return 0;          // No Connect
    case 1:  return MODE_U1RTS;
    case 2:  return MODE_U2TX;
    case 3:  return MODE_U5RTS;
    case 4:  return MODE_U6TX;
    case 5:  return 0;          // Reserved
    case 6:  return MODE_SS2;
    case 7:  return 0;          // Reserved
    case 8:  return MODE_SDO4;
    case 9:  return 0;          // Reserved
    case 10: return MODE_SDO6;
    case 11: return MODE_OC2;
    case 12: return MODE_OC1;
    case 13: return MODE_OC9;
    case 14: return 0;          // Reserved
    case 15: return MODE_C2TX;
    }
}

gpio_mode_t gpio_get_output_mapping(int pin)
{
    switch (pin) {
    case GPIO_PIN('D',2):  return mode_output_group1(read_sfr(RPD2R));
    case GPIO_PIN('G',8):  return mode_output_group1(read_sfr(RPG8R));
    case GPIO_PIN('F',4):  return mode_output_group1(read_sfr(RPF4R));
    case GPIO_PIN('F',1):  return mode_output_group1(read_sfr(RPF1R));
    case GPIO_PIN('B',9):  return mode_output_group1(read_sfr(RPB9R));
    case GPIO_PIN('B',10): return mode_output_group1(read_sfr(RPB10R));
    case GPIO_PIN('B',5):  return mode_output_group1(read_sfr(RPB5R));
    case GPIO_PIN('C',1):  return mode_output_group1(read_sfr(RPC1R));
    case GPIO_PIN('D',14): return mode_output_group1(read_sfr(RPD14R));
    case GPIO_PIN('G',1):  return mode_output_group1(read_sfr(RPG1R));
    case GPIO_PIN('A',14): return mode_output_group1(read_sfr(RPA14R));
    case GPIO_PIN('D',6):  return mode_output_group1(read_sfr(RPD6R));

    case GPIO_PIN('D',3):  return mode_output_group2(read_sfr(RPD3R));
    case GPIO_PIN('G',7):  return mode_output_group2(read_sfr(RPG7R));
    case GPIO_PIN('F',5):  return mode_output_group2(read_sfr(RPF5R));
    case GPIO_PIN('D',11): return mode_output_group2(read_sfr(RPD11R));
    case GPIO_PIN('F',0):  return mode_output_group2(read_sfr(RPF0R));
    case GPIO_PIN('B',1):  return mode_output_group2(read_sfr(RPB1R));
    case GPIO_PIN('E',5):  return mode_output_group2(read_sfr(RPE5R));
    case GPIO_PIN('B',3):  return mode_output_group2(read_sfr(RPB3R));
    case GPIO_PIN('C',4):  return mode_output_group2(read_sfr(RPC4R));
    case GPIO_PIN('G',0):  return mode_output_group2(read_sfr(RPG0R));
    case GPIO_PIN('A',15): return mode_output_group2(read_sfr(RPA15R));
    case GPIO_PIN('D',7):  return mode_output_group2(read_sfr(RPD7R));

    case GPIO_PIN('D',9):  return mode_output_group3(read_sfr(RPD9R));
    case GPIO_PIN('B',8):  return mode_output_group3(read_sfr(RPB8R));
    case GPIO_PIN('B',15): return mode_output_group3(read_sfr(RPB15R));
    case GPIO_PIN('D',4):  return mode_output_group3(read_sfr(RPD4R));
    case GPIO_PIN('B',0):  return mode_output_group3(read_sfr(RPB0R));
    case GPIO_PIN('E',3):  return mode_output_group3(read_sfr(RPE3R));
    case GPIO_PIN('B',7):  return mode_output_group3(read_sfr(RPB7R));
    case GPIO_PIN('F',12): return mode_output_group3(read_sfr(RPF12R));
    case GPIO_PIN('D',12): return mode_output_group3(read_sfr(RPD12R));
    case GPIO_PIN('F',8):  return mode_output_group3(read_sfr(RPF8R));
    case GPIO_PIN('C',3):  return mode_output_group3(read_sfr(RPC3R));
    case GPIO_PIN('E',9):  return mode_output_group3(read_sfr(RPE9R));

    case GPIO_PIN('G',9):  return mode_output_group4(read_sfr(RPG9R));
    case GPIO_PIN('D',0):  return mode_output_group4(read_sfr(RPD0R));
    case GPIO_PIN('B',6):  return mode_output_group4(read_sfr(RPB6R));
    case GPIO_PIN('D',5):  return mode_output_group4(read_sfr(RPD5R));
    case GPIO_PIN('B',2):  return mode_output_group4(read_sfr(RPB2R));
    case GPIO_PIN('F',3):  return mode_output_group4(read_sfr(RPF3R));
    case GPIO_PIN('C',2):  return mode_output_group4(read_sfr(RPC2R));
    case GPIO_PIN('E',8):  return mode_output_group4(read_sfr(RPE8R));
    case GPIO_PIN('F',2):  return mode_output_group4(read_sfr(RPF2R));
    }
    return 0;
}

gpio_mode_t gpio_get_input_mapping(int pin)
{
    //TODO: get input mapping
    return 0;
}
