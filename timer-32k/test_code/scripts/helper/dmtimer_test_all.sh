#!/bin/sh

`insmod $TESTMODS/dmtimer_test_all.ko`
RET=$?

if [ "$RET" = "1" ]; then
	exit 1
else
	`rmmod $TESTMODS/dmtimer_test_all.ko`
	exit $?
fi
