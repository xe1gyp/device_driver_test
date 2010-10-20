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
  insmod $McBSP_MODULE $COMMAND$VALUE test_mcbsp_id=$i
  print "Suspending Driver..."
  print "echo suspend > /proc/driver/mcbsp_test/transmission"
  sleep $MESSAGE_DELAY
  echo 'suspend' > /proc/driver/mcbsp_test/transmission
  print "Trying Transmission..."
  print "echo start > /proc/driver/mcbsp_test/transmission"
  sleep $MESSAGE_DELAY
  echo 'start' > /proc/driver/mcbsp_test/transmission
  TX=`cat /proc/driver/mcbsp_test/status | grep "No. of buffers transmitted" | awk '{print $6}'`
  RX=`cat /proc/driver/mcbsp_test/status | grep "No. of buffers received" | awk '{print $6}'`
  if [ $TX -ne 0 ] || [ $RX -ne 0 ] 
  then
    rmmod $McBSP_MODULE
    print "Tranmission on McBSP Interface $i is available after suspending"
    print "Tx Value = $TX | Rx Value = $RX"
    print "FAIL"            
    sleep $MESSAGE_DELAY
    exit 1
  fi
  print "Tranmission on McBSP Interface $i is not available"
  print "Tx Value = $TX | Rx Value = $RX"
  sleep $MESSAGE_DELAY
  print "Resuming Driver..."
  sleep $MESSAGE_DELAY
  print "Resuming Transmission \"echo resume > /proc/driver/mcbsp_test/transmission\""
  echo 'resume' > /proc/driver/mcbsp_test/transmission
  print "Starting Transmission \"echo start > /proc/driver/mcbsp_test/transmission\""    
  echo 'start' > /proc/driver/mcbsp_test/transmission
  sleep $DELAY
     
  TX=`cat /proc/driver/mcbsp_test/status | grep "No. of buffers transmitted" | awk '{print $6}'`
  RX=`cat /proc/driver/mcbsp_test/status | grep "No. of buffers received" | awk '{print $6}'`
    
  if [ $TX -ne 100 ] || [ $RX -ne 100 ] 
  then
    print "Tx Value = $TX | $RX = Rx Value"     
    print "Failed Tranmission on McBSP Interface $i after resuming"
    print "FAIL"
    rmmod $McBSP_MODULE      
    exit $MESSAGE_DELAY    
  else
    print "Tx Value = $TX | $RX = Rx Value"    
    print "Succesful Tranmission on McBSP Interface $i after resuming"
    print "PASS"
    rmmod $McBSP_MODULE      
    sleep $MESSAGE_DELAY      
  fi
done
exit 0

