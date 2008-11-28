#!/bin/sh

x=1
LIMIT=255
status=0

while [ $x -lt $LIMIT ]
do
  x=`expr $x + 1`
  data=`printf "%X" $x`
  echo "write 01 $data" > $HDQ_SYSFS_CMD
  echo "read 01" > $HDQ_SYSFS_CMD
  rx=`cat $HDQ_SYSFS_RESULT`

  if [ "$rx" != "Inval" ]
  then
    echo "Transmitted $data <> Expected Inval <> Received $rx <> FAIL"
    status=1
  else 
    echo "Transmitted $data <> Expected Inval <> Received $rx <> PASS"
  fi
done

if [ "$status" -eq 1 ] ; then
  exit 1
else 
  exit 0
fi
