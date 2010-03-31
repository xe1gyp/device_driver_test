#!/bin/sh
set -x
error=0
COUNT=$FIRST_GPIO_LINE
while [ $COUNT -le $MAX_GPIO_LINES ]
do
	insmod $TEST_MODULE test=5 gpio=$COUNT value=1
	cat /proc/driver/gpio_test_result | grep FAILED
	if [ $? -eq 0 ]
	then
		error=`expr $error + 1`
	fi
	rmmod $TEST_MODULE
	COUNT=`expr $COUNT + 1`
done

if [ $error -gt 0 ]
then
	echo " The number of errors = $error, Please refer the log for details "
	return 1
else
	return 0
fi
