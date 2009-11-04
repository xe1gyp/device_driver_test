#!/bin/sh
set -x
error=0
COUNT=$FIRST_GPIO_LINE
while [ $COUNT -le $MAX_GPIO_LINES ]
do
	if [ $COUNT -ne 152 ]
	then
		insmod $TEST_MODULE test=4 gpio=$COUNT value=1
		cat /proc/driver/gpio_test_result | grep FAILED
		if [ $? -eq 0 ]
		then
			error=`expr $error + 1`
		fi
		rmmod $TEST_MODULE
	fi
	COUNT=`expr $COUNT + 1`
done

if [ $error -gt 0 ]
then
	echo " The number of errors = $error, Please refer the log for more details "
	exit 1
else
	exit 0
fi
