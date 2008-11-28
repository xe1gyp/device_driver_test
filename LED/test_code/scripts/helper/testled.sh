#!/bin/sh

print()
{
  echo $1
  echo $1 >> $LOG_OUTPUT
}

TAG=$1
PARAMETERS=$2
DESCRIPTION=$3

print ""
print "----------------------- L_DD_TWL4030LED_$TAG -----------------------"
print "$DESCRIPTION"
print "insmod $TWL4030LED_MODULE $PARAMETERS"
sleep $MESSAGE_DELAY

insmod $TWL4030LED_MODULE $PARAMETERS

ERR=$?
if [ $ERR -eq -1 ]; then
  print "Module could not be installed"
  print "Returned Error $ERR"
	exit 1
else
  print "Module Installed"
fi

if [ -z "$STRESS" ]; then
  print "Is LED working according to the description?"
  $WAIT_ANSWER
  ERR=$?
  if [ $ERR -eq 1 ]; then
    print "TWL4030LED visual inspection failed"
    print "FAIL"
    exit 1
  else
    print "TWL4030LED visual inspection passed"
    print "PASS"
    exit 0
  fi
fi
