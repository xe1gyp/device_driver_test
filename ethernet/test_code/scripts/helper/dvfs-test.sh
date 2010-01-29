#!/bin/sh
$UTILSCRIPTS/fixed.opp.sh &
ping -c 1 `cat $TMPBASE/ipaddr`
ping -c 5 `cat $TMPBASE/ipaddr`
ping -c 10 `cat $TMPBASE/ipaddr`
ping -c 15 `cat $TMPBASE/ipaddr`
killall fixed.opp.sh
