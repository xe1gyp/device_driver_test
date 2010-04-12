#!/bin/sh

NAME=$1
PARAMETERS=$2
TIME_TO_REPEAT=$3
MESSAGE=$4

ans=0
counter=0

print()
{
  msg=$1
  echo $msg
  echo $msg >> $LOG_OUTPUT
}

result()
{
  if [ -z "$STRESS" ]; then
    print "Is Power Management working according to the description?"
    $WAIT_ANSWER
    ERR=$?
    if [ $ERR -eq 1 ]; then
      print "FAIL"
      ans=1
    else
      print "PASS"
    fi
  fi
}

print ""
print "---------------------- L_DD_PM_$NAME ----------------------"
print "$MESSAGE"

while [ $counter -lt $TIME_TO_REPEAT ]
do
  counter=`expr $counter + 1`
  echo -e "$PARAMETERS"|$DPM_SCRIPT
done

print "Was Power Management Framework interaction according to specifications?"
result

if [ $ans -eq 1 ]; then
    print "Errors found!"
    exit 1
else
    print "All Passed!"
    exit 0
fi

