#!/bin/sh

COMMENT=$1

if [ -z "$STRESS" ]; then
	echo "";echo $COMMENT;echo ""
	$WAIT_KEY
else
	echo "";echo $COMMENT;echo ""
fi
