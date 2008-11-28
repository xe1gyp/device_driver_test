#!/bin/sh

NAME=$1
TAG=$2
PLACE=$3
COMMAND=$4
VARIABLE=$5

print()
{
  msg=$1
  echo $msg
}

print ""
print "----------------------- Test Case Scenario $NAME -----------------------"

for i in $McBSP_INTERFACES
do

  print ""
  print "----- McBSP Interface $i"
  print "----- Parameter \"$TAG\"" 
  print "----- Available values .: $VARIABLE :."

  for j in $VARIABLE
  do
    print ""
    print "Testing McBSP Interface $i with $COMMAND$j"
    sleep $MESSAGE_DELAY
    insmod $McBSP_MODULE $COMMAND$j test_mcbsp_id=$i
    TEMP=`cat /proc/driver/mcbsp_test/status | grep "$TAG" | sed -e "s/ */ /g" | cut -d ' ' -f$PLACE`
    print "Starting Transmission \"echo start > /proc/driver/mcbsp_test/transmission\""
    sleep $MESSAGE_DELAY    
    echo 'start' > /proc/driver/mcbsp_test/transmission
    sleep $DELAY
    print "Cancelling Transmission..."
    print "echo stop > /proc/driver/mcbsp_test/transmission"
    echo 'stop' > /proc/driver/mcbsp_test/transmission
    sleep $MESSAGE_DELAY
    print "Waiting $MESSAGE_DELAY seconds and verifying if Transmission was really cancelled..."
    sleep $MESSAGE_DELAY
    TX=`cat /proc/driver/mcbsp_test/status | grep "No. of buffers transmitted" | sed -e "s/ */ /g" | cut -d ' ' -f7`
    RX=`cat /proc/driver/mcbsp_test/status | grep "No. of buffers received" | sed -e "s/ */ /g" | cut -d ' ' -f7`
    if [ "$TX" -lt "$TEMP" ] || [ "$RX" -lt  "$TEMP" ]
    then
      print "Tx Value = $TX | $RX = Rx Value"    
      print "Succesful Suspend on Transmission McBSP Interface $i using $COMMAND$j"
      print "PASS"
      rmmod $McBSP_MODULE
    else
      print "Tx Value = $TX | $RX = Rx Value"          
      print "Failed Suspend on Transmission McBSP Interface $i using $COMMAND$j"
      print "FAIL"
      rmmod $McBSP_MODULE
      exit 1      
    fi
  done
done

exit 0
