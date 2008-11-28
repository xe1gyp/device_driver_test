#!/bin/sh

NAME=$1
STATES=$2
SETIMG_PARAMETERS=$3
STREAMING_PARAMETERS=$4
MESSAGE=$5

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
  sleep 5
  if [ -z "$STRESS" ]; then
    akey
    ERR=$?
    if [ $ERR -eq 1 ]; then
      print "FAIL"
      ans=1
    else
      print "PASS"
    fi
  fi
}

put_on_state()
{
  print ""
  if [ "$1" != "$SUSPEND" ]; then
    print "Power State $1 <> Streaming video shall continue"
  else
    print "Power State $1 <> Streaming video shall not continue"
  fi
  echo -n $1 > $VIDEO_1_PM
  sleep 5
}

set $SETIMG_PARAMETERS

print ""
print "---------------------- L_DD_VIDEO_$TAG ----------------------"
print "Let's execute Dynamic Power Management for Video-$1"
print "Setting image parameters"
print "<vid:$1> <fmt:$2> <width:$3> <height:$4>"
sleep $MESSAGE_DELAY

$TESTBIN/setimg $SETIMG_PARAMETERS
cd $TESTBIN
eval $TESTBIN/streaming $STREAMING_PARAMETERS &

#cd $TESTBIN
#$TESTBIN/ioctl $IOCTL_PARAMETERS
#eval $COMMAND &

set $STATES
sleep 3

case "$1" in 
    "DPM") 
            while [ $counter -lt $2 ]
            do
              counter=`expr $counter + 1`
              echo -e "$SCALE_OPTION\n$QUIT_OPTION"|$DPM_SCRIPT;
              sleep 5
            done
         ;; 
    "SLEEP")
            while [ $counter -lt $2 ]
            do
              counter=`expr $counter + 1`
              if [ -z "$STRESS" ]; then
                echo -e "$SLEEP_OPTION\n$QUIT_OPTION"|$DPM_SCRIPT;
              fi
              sleep 5
            done
         ;;
    *) 
            for st in $STATES
            do
              put_on_state $st
            done
         ;; 
esac


killall streaming
print "Killing streaming... wait!"
print "Was the Streaming video-1 interacting with Power Management Framework according to specifications?"
result

if [ $ans -eq 1 ]; then
    print "Errors found!"
    exit 1
else
    print "All Passed!"
    exit 0
fi
