#!/bin/sh
REQUESTED_TYPE=$1

`insmod $TESTMODS/gptimer_request.ko clock_type=$REQUESTED_TYPE`
RET=$?

if [ "$RET" = "1" ]; then
	exit 1
else
	`rmmod $TESTMODS/gptimer_request.ko`
	exit $?
fi
