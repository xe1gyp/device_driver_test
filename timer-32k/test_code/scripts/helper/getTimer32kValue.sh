#!/bin/sh

QUERY_DELAY=$1

`insmod $TIMER_DIR_MODULES/timer32value.ko delay=$QUERY_DELAY`
RET=$?

if [ "$RET" = "1" ]; then
	exit 1
else
	`rmmod timer32value`
	exit $?
fi

# End of file
