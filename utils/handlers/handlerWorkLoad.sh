#!/bin/sh

set -x

LOCAL_COMMAND=$1

if [ "$LOCAL_COMMAND" = "start" ]; then

	dd if=/dev/urandom of=/dev/null &

elif [ "$LOCAL_COMMAND" = "stop" ]; then

	pid=`ps -x | grep dd | grep /dev/null | awk '{print $1}'`
	kill $pid

fi

# End of file
