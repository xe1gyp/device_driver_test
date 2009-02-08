#!/bin/sh

ADAPTER_INFO=`cat $DMESG_PATH | grep i2c | grep "bus 1"`
SPEED_RECEIVED=$1
SPEED_ACTUAL=`echo $ADAPTER_INFO | sed -e "s/ */ /g" | cut -d ' ' -f8`

if [ "$SPEED_RECEIVED" = "$SPEED_ACTUAL" ]; then
	return 0
else
	echo
	echo "This is not an issue, adapter speed is not set to the required one!"
	echo "Change speed in kernel configuration and run again"
	echo "Requesting -> $SPEED_RECEIVED"
	echo "Actual -> $SPEED_ACTUAL"
	echo $ADAPTER_INFO
	echo
	return 1
fi

# End of file
