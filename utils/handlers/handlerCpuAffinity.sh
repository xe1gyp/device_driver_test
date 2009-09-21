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

		if [ ! -d "/proc/$LOCAL_PID" ]
		then
			break
		fi

		rem=$(( $count % 2 ))

		if [ $rem -eq 0 ]
		then
			processor=1
			$UTILBIN/taskset -p $processor $LOCAL_PID
		else
			processor=2
			$UTILBIN/taskset -p $processor $LOCAL_PID
		fi

		if [ $? -eq 1 ]
		then
		  echo "Cannot set affinity for processor " $processor
		  return 1
		fi

		count=`expr $count + 1`
		echo "$LOCAL_PID | $count"
		sleep 5

	done

	continue

fi
