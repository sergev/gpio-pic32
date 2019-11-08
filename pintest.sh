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


# testPin pin
################################################################################

testPin()
{
  start=$1
    end=$2
   errs=0

  printf "%20s: "  "Pin p$1"

  # Set pin to inputs
  gpio mode p$1 in

  # Enable internal pull-up and expect to read high
  gpio mode p$1 up
  if [ `gpio read p$1` = 0 ]; then
    logErr p$1 1 0
  fi

  # Enable internal pull-down and expect to read low
  gpio mode p$1 down
  if [ `gpio read p$1` = 1 ]; then
    echo "Pin p$1 failure - expected 0, got 1"
    let errs+=1
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

# Start here
################################################################################

   errs=0
totErrs=0

echo ""

# Main pins

testPin 0
echo "              Pin p1:  Not present in hardware"

# I2C - Needs somewhat different testing
#	due to the on-board pull-up's
echo -n "  The I2C pins p2-p3: "
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
echo "             Pin p15:  Not tested (U4RX)"
testPin 16
testPin 17
testPin 18
echo "             Pin p19:  Not tested (U2RX)"
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
