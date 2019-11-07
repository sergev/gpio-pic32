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


# logErr pin, expected got
################################################################################

logErr ()
{
  if [ $errs = 0 ]; then
    echo ""
  fi
  echo " --> Pin $1 failure. Expected $2, got $3"
  let errs+=1
}


# printErrorCount
################################################################################

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


# testPins start end
################################################################################

testPins()
{
  start=$1
    end=$2
   errs=0

  printf "%30s p%-2d:p%-2d: "  "$3" $1 $2

# Set range to inputs

  for i in `seq $start $end`; do
    gpio mode p$i in
  done

# Enable internal pull-ups and expect to read high

  for i in `seq $start $end`; do
    gpio mode p$i up
    if [ `gpio read p$i` = 0 ]; then
      logErr p$i 1 0
    fi
  done

# Enable internal pull-downs and expect to read low

  for i in `seq $start $end`; do
    gpio mode p$i down
    if [ `gpio read p$i` = 1 ]; then
      echo "Pin p$i failure - expected 0, got 1"
      let errs+=1
    fi
  done

# Remove the internal pull up/downs

  for i in `seq $start $end`; do
    gpio mode p$i tri
  done

  if [ $errs = 0 ]; then
    echo " OK"
  else
    printErrCount
  fi

  let totErrs+=errs
}


intro()
{
  cat <<EOF
PinTest
=======

This is a simple utility to test the GPIO pins on your PIC32 Starter Kit.

NOTE: All GPIO peripherals must be removed to perform this test. This
  includes serial, I2C and SPI connections. You may get incorrect results
  if something is connected and it interferes with the test.

This test can only test the input side of things. It uses the internal
pull-up and pull-down resistors to simulate inputs. It does not test
the output drivers.

Please make sure everything is removed and press the ENTER key to continue,
EOF

  echo -n "or Control-C to abort... "
  read a
}


# Start here
################################################################################

intro

   errs=0
totErrs=0

echo ""

# Main pins

testPins 0 0 "Pin"

testPins 4 27 "The main 24 GPIO pins"

# I2C - Needs somewhat different testing
#	due to the on-board pull-up's

echo -n "                  The I2C pins p2 :p3 : "
errs=0
gpio mode p2 in
gpio mode p3 in

if [ `gpio read p2` = 0 ]; then
  echo "Pin p2 failure - expected 1, got 0"
  let errs+=1
fi

if [ `gpio read p3` = 0 ]; then
  echo "Pin p3 failure - expected 1, got 0"
  let errs+=1
fi

if [ $errs = 0 ]; then
  echo " OK"
else
  printErrCount
fi

echo ""
if [ $totErrs != 0 ]; then
  echo ""
  echo "Faults detected! Output of 'readall':"
  gpio readall
fi
