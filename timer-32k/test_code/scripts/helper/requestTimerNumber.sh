#!/bin/sh
REQUESTED_GPTIMER=$1

if [ "$REQUESTED_GPTIMER" -eq "$RESERVED_GPTIMER" ]; then
	echo "A GPtimer was requested when it was already reserved."
	echo "Module insertion is not going to be performed"
	exit 0
fi

`insmod $TIMER_DIR_MODULES/gptimer_request_specific.ko gptimer_id=$REQUESTED_GPTIMER`
RET=$?

if [ "$RET" -eq "1" ]; then
	exit 1
else
	`rmmod $TIMER_DIR_MODULES/gptimer_request_specific.ko`
	exit $?
fi

# End of file
