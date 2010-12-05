#!/bin/sh

`insmod $TIMER_DIR_MODULES/dmtimer_test_all.ko`
RET=$?

if [ "$RET" = "1" ]; then
	exit 1
else
	`rmmod dmtimer_test_all`
	exit $?
fi

# End of file
