#!/bin/bash
#
# Test the GPIO port of PIC32 Starter Kit
#
# Copyright (c) 2013-2015 Gordon Henderson
# Copyright (c) 2019 Serge Vakulenko
#################################################################################
#
#    This file is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This file is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details,
#   <http://www.gnu.org/licenses/>
#
#################################################################################

#
# logErr pin, expected got
#
logErr ()
{
    if [ $errs = 0 ]; then
        echo ""
    fi
    echo " --> Pin $1 failure. Expected $2, got $3"
    let errs+=1
}

#
# printErrorCount
#
printErrCount()
{
    if [ $errs = 0 ]; then
        echo "No faults detected."
    elif [ $errs = 1 ]; then
        echo "One fault detected."
    else
        echo "$errs faults detected"
    fi
}

#
# testPin pin [pullup]
#
testPin()
{
    start=$1
    pullup=$2
    errs=0

    if [ -z "$pullup" ]; then
        printf "%20s: "  "Pin p$1"
    else
        printf "%20s: "  "Pin p$1 with pull-up"
    fi

    # Set pin to output
    gpio mode p$1 out

    # Set pin high and expect to read high
    gpio write p$1 1
    if [ `gpio read p$1` = 0 ]; then
        logErr p$1 1 0
    fi

    # Set pin low and expect to read low
    gpio write p$1 0
    if [ `gpio read p$1` = 1 ]; then
        echo "Pin p$1 failure - expected 0, got 1"
        let errs+=1
    fi

    # Set pin to input
    gpio mode p$1 in

    # Enable internal pull-up and expect to read high
    gpio mode p$1 up
    if [ `gpio read p$1` = 0 ]; then
        logErr p$1 1 0
    fi

    # Enable internal pull-down and expect to read low
    if [ -z "$pullup" ]; then
        gpio mode p$1 down
        if [ `gpio read p$1` = 1 ]; then
            echo "Pin p$1 failure - expected 0, got 1"
            let errs+=1
        fi
    fi

    # Remove the internal pull up/down
    gpio mode p$1 tri

    if [ $errs = 0 ]; then
        echo " OK"
    else
        printErrCount
    fi

    let totErrs+=errs
}

#
# Start here
#
errs=0
totErrs=0
echo ""

# Switch u2rx to p19 (default)
gpio mode p19 u2rx

testPin 0
echo "              Pin p1:  Not present in hardware"

# I2C - Needs somewhat different testing
#	due to the on-board pull-up's
testPin 2 pullup
testPin 3 pullup

testPin 4
testPin 5
testPin 6
testPin 7
testPin 8
testPin 9
testPin 10
testPin 11
testPin 12
testPin 13
testPin 14
testPin 15
testPin 16
testPin 17
testPin 18

# U2RX - Needs somewhat different testing
#	 due to the on-board pull-up
gpio mode p3 u2rx   # Switch u2rx to p3, which also has a pull-up, so it's safe
testPin 19 pullup
gpio mode p19 u2rx  # Switch u2rx back to p19

testPin 20
testPin 21
testPin 22
testPin 23
testPin 24
testPin 25
testPin 26
testPin 27

echo ""
if [ $totErrs != 0 ]; then
    echo ""
    echo "Faults detected! Output of 'readall':"
    gpio readall
fi
