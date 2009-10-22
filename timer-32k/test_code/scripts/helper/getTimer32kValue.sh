#!/bin/sh
QUERY_DELAY=$1

`insmod $TESTMODS/timer32value.ko delay=$QUERY_DELAY`
RET=$?

if [ "$RET" = "1" ]; then
	exit 1
else
	`rmmod $TESTMODS/timer32value.ko`
	exit $?
fi
