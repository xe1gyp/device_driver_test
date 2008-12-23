#!/bin/sh

echo "Was the video streamed correctly with the desired options?"
$WAIT_ANSWER
ERR=$?
if [ $ERR -eq 1 ]; then
	echo "FAIL"
        exit 1
else
	echo "PASS"
        exit 0
fi
