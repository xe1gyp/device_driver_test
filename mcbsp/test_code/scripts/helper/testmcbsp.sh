#!/bin/sh

NAME=$1
TAG=$2
PLACE=$3
COMMAND=$4
VARIABLE=$5
status=0

print()
{
  msg=$1
  echo $msg
}

time()
{
	cat /proc/driver/mcbsp_test/status | grep "Time taken"
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
    if [ "$COMMAND" = "no_of_trans=" ] ; then
      TRANSFER=$j
    else
      TRANSFER=100
    fi
    print ""
    print "Testing McBSP Interface $i with $COMMAND$j"
    sleep $MESSAGE_DELAY
    if [ "$COMMAND" == "" ]
    then
      insmod -f $McBSP_MODULE test_mcbsp_id=$i
      TEMP=$j
    else
      insmod -f $McBSP_MODULE $COMMAND$j test_mcbsp_id=$i
      TEMP=`cat /proc/driver/mcbsp_test/status | grep "$TAG" | sed -e "s/ */ /g" | cut -d ' ' -f$PLACE`
    fi
    print "Starting Transmission : \"echo start > /proc/driver/mcbsp_test/transmission\""
    sleep $MESSAGE_DELAY
    echo 'start' > /proc/driver/mcbsp_test/transmission
    sleep $DELAY
    TX=`cat /proc/driver/mcbsp_test/status | grep "No. of buffers transmitted" | sed -e "s/ */ /g" | cut -d ' ' -f7`
    RX=`cat /proc/driver/mcbsp_test/status | grep "No. of buffers received" | sed -e "s/ */ /g" | cut -d ' ' -f7`
    time
    if [ "$TEMP" != "$j" ] || [ "$TX" != "$TRANSFER" ] || [ "$RX" != "$TRANSFER" ] 
    then
      print "Tx Value = $TX | $RX = Rx Value"
      print "Failed Tranmission on McBSP Interface $i using $COMMAND$j"
      print "FAIL"
      sleep $MESSAGE_DELAY
      rmmod $McBSP_MODULE
      status=1
    else
      print "Tx Value = $TX | $RX = Rx Value"      
      print "Succesful Tranmission on McBSP Interface $i using $COMMAND$j"
      print "PASS"
      sleep $MESSAGE_DELAY
      rmmod $McBSP_MODULE      
    fi
  done
done


if [ "$status" -eq 1 ] ; then
  exit 1
else 
  exit 0
fi
