#!/bin/sh
set -x
error=0

insmod $TEST_MODULE test=7
cat /proc/driver/gpio_test_result | grep FAILED
if [ $? -eq 0 ]
then
	error=`expr $error + 1`
fi
rmmod $TEST_MODULE

if [ $error -gt 0 ]
then
	echo " Error, please refer the log for more details "
	return 1
else
	return 0
fi
