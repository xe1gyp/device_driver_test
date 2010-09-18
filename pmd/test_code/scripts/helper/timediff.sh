#!/bin/sh

if [ $# -ne 3 ]
then
	echo " "
	echo "Usage: timediff.sh <time before suspend> <time after wakeup> <wakeup time>"
	echo " "
	exit 1
fi

tdiff=`expr $2 - $1`

delta=`expr $tdiff - $3`

if [ $delta -lt  5 ]
then
	echo "TEST PASSED"
else
	echo "TEST FAILED"
fi

# End of file
