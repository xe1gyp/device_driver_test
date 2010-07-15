#!/bin/sh

if [ $# -ne 2 ]
then
	echo " "
	echo "Usage: locCount.sh <loc before sleep> <loc after sleep>"
	echo " "
	exit 1
fi

loc=`expr $2 - $1`;

if [ $loc -gt 1500 ]
then
	echo "TEST FAILED"
else
	echo "TEST PASSED"
fi
