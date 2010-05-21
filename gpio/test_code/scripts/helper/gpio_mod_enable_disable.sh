#!/bin/sh
set -x
error=0

insmod $TEST_MODULE test=9
cat /proc/driver/gpio_test_result | grep FAILED
if [ $? -eq 0 ]
then
	error=`expr $error + 1`
fi
rmmod $TEST_MODULE

if [ $error -gt 0 ]
then
	echo " Error, please refer the log for more details "
	exit 1
else
	exit 0
fi
