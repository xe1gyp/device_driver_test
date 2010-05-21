#!/bin/sh
set -x
error=0
COUNT=$FIRST_GPIO_LINE
while [ $COUNT -le $MAX_GPIO_LINES ]
do
	$UTILS_DIR_HANDLERS/handlerCpuAffinity.sh "switch" "insmod $TEST_MODULE test=6  gpio=$COUNT  value=1  iterations=20 " "1" "1"
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
	exit 1
else
	exit 0
fi
