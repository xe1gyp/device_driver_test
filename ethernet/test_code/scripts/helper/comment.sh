#!/bin/sh

COMMENT=$1

if [ -z "$STRESS" ]; then
	echo "";echo $COMMENT;echo ""
	# We'll disable for now Stress Mode for Ethernet driver
	#$WAITKEY
else 
	echo "";echo $COMMENT;echo ""
fi
