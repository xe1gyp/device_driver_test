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
#    sleep $MESSAGE_DELAY
#    INSERTED=`lsmod | grep omap_mcbsp_test | cut -d ' ' -f1`
#    print "$INSERTED $MODNAME"
#    if [ $INSERTED == $MODNAME ]
#    then
#	sleep $DELAY
#        print "Module was already inserted. Removing it..."
#	rmmod $MODNAME
#    fi
    insmod $McBSP_MODULE $COMMAND$j test_mcbsp_id=$i
    TEMP=`cat /proc/driver/mcbsp_test/status | grep "$TAG" | awk '{print $6}'`
    print "Starting Transmission"
    sleep $MESSAGE_DELAY
    echo 'start' > /proc/driver/mcbsp_test/transmission &
    sleep $DELAY
    print "Cancelling Transmission..."
    sleep $MESSAGE_DELAY
    echo 'stop' > /proc/driver/mcbsp_test/transmission
    TX=`cat /proc/driver/mcbsp_test/status | grep "No. of buffers transmitted" | awk '{print $6}'`
    RX=`cat /proc/driver/mcbsp_test/status | grep "No. of buffers received" | awk '{print $6}'`
    if [ $TX -lt $j  ]
    then
      print "Tx Value = $TX | $RX = Rx Value"
      print "Succesful cancellation of data transfer on McBSP Interface $i using $COMMAND$j"
      print "PASS"
      rmmod $McBSP_MODULE
    else
      print "Tx Value = $TX | $RX = Rx Value"
      print "Failed cancellation of data transfer on McBSP Interface $i using $COMMAND$j"
      print "FAIL"
      rmmod $McBSP_MODULE
      exit 1
    fi
  done
done

exit 0
