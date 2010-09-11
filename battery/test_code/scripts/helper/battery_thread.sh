#!/bin/sh
# (C) Texas Instruments 2009
# Initial Version 21.Oct.2009
#
# battery_thread.sh - Access Battery params from multiple threads

if [ $# != 2 ]
then
	echo "***Error: Missing argument"
	echo "     Invoke as $0 <Test Platform type> <numiterations>"
	echo "     Example: $0 OMAP 400"
	exit -1
fi

export TEST_PLATFORM=$1
#Include the common utilities needed for Battery tests
. battery_util.sh

#Global variables
Global_test_result=0 # Set Pass
Global_ERR=0

access_battery_params()
{
	loopcount=0
	loopmax=$1
	while [ $loopcount != $loopmax ]
	do
		battery_test_get_battery_state retval1 retval2 retval3 retval4 retval5
		loopcount=`expr $loopcount + 1`
		if [ $Global_ERR != 0 ]
		then
			echo "Error encountered: $$ failed on $loopcount iteration"
			loopcount=$loopmax
		fi
	done
	echo "Pid $$ done"
}

access_battery_params $2
