#!/bin/sh

REQUESTED_TYPE=$1

`insmod $TIMER_DIR_MODULES/gptimer_request.ko clock_type=$REQUESTED_TYPE`
RET=$?

if [ "$RET" = "1" ]; then
	exit 1
else
	`rmmod $TIMER_DIR_MODULES/gptimer_request.ko`
	exit $?
fi

# End of file
