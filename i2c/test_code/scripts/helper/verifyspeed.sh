#!/bin/sh

system_message_buffer.sh "system.log" || exit 1
ADAPTER_INFO=`cat "system.log" | grep i2c | grep "bus 1"`
SPEED_ACTUAL=`echo $ADAPTER_INFO | sed -e "s/ */ /g" | cut -d ' ' -f8`
rm "system.log" || exit 1

echo ""
echo "Working with Bus 1 with speed $SPEED_ACTUAL"
echo "$ADAPTER_INFO"
echo ""

# End of file
