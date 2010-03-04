#!/bin/sh

ADAPTER_INFO=`cat /tmp/result.tmp | grep i2c | grep "bus 1"`
SPEED_ACTUAL=`echo $ADAPTER_INFO | sed -e "s/ */ /g" | cut -d ' ' -f8`

echo
echo "Working with Bus 1 with speed $SPEED_ACTUAL"
echo $ADAPTER_INFO
echo

# End of file