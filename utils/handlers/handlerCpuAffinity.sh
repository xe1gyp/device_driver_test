#!/bin/sh

set -x

LOCAL_COMMAND=$1
LOCAL_PROGRAM=$2
LOCAL_TIMES=$3

if [ "$LOCAL_COMMAND" = "switch" ]; then

	eval $LOCAL_PROGRAM &
	LOCAL_PID=`echo $!`
	
	count=0

	while [ $count -lt $LOCAL_TIMES ]
	do	
		rem=$(( $count % 2 ))
		if [ $rem -eq 0 ]
		then
			echo "$count is even number"
			$UTILBIN/taskset -p 1 $LOCAL_PID
		else
			echo "$count is odd number"
			$UTILBIN/taskset -p 2 $LOCAL_PID
		fi
	
		count=`expr $count + 1`
		echo "$LOCAL_PID | $count"
		sleep 10
	done

fi
