#!/bin/sh
# (C) Texas Instruments 2009
# Initial Version 21.Oct.2009
#
# battery_gen.sh - Generic Battery health testcases

if [ $# == 0 ]
then
	echo "Error: Missing argument"
	echo "     Invoke as $0 <Test Platform type>"
	echo "     Example: $0 OMAP"
	exit -1
fi

export TEST_PLATFORM=$1
#Include the common utilities needed for Battery tests
. battery_util.sh

#Global variables
Global_test_result=0 # Set Pass
Global_ERR=0
retval1=
retval2=
retval3=
retval4=
retval5=

battery_test_get_battery_state retval1 retval2 retval3 retval4 retval5
echo "Battery params $retval1 $retval2 $retval3 $retval4 $retval5"

Global_test_result=$Global_ERR
if [ $Global_test_result == 0 ]
then
	echo "General Battery tests PASSED"
else
	echo "General Battery tests FAILED"
fi
