#!/bin/sh

print()
{
  echo $1
  echo $1 >> $LOG_OUTPUT
}

NAME=$1
PARAMETERS=$2
DESCRIPTION=$3

print ""
print "----------------------- Test Case Scenario $NAME -----------------------"
print "$DESCRIPTION"
print "insmod $SOUNDSER_MODULE $PARAMETERS"
sleep $MESSAGE_DELAY

insmod $SOUNDSER_MODULE $PARAMETERS

ERR=$?
if [ $ERR -eq -1 ]; then
  rmmod $SOUNDSER_MODULE
  print "Module could not be installed"
  print "Returned Error $ERR"
	exit 1
else
  print "Module Installed"
fi

print "Sysfs entry is displayed below this message >"
cat $SYSFSENTRY
sleep 1
rmmod $SOUNDSER_MODULE

if [ -z "$STRESS" ]; then
  print "Did you notice the $CONTROL_TYPE changing?"
  $WAIT_ANSWER
  ERR=$?
  if [ $ERR -eq 1 ]; then
    print "Sound Services visual inspection failed"
    print "FAIL"
    exit 1
  else
    print "Sound Services visual inspection passed"
    print "PASS"
    exit 0
  fi
fi

