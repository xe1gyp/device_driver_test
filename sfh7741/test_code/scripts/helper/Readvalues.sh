#!/bin/sh



command=$2
i=$1
while [ 0 -le $i ]
do
  echo  $command
  #Check command
  if [ "$command" = "1" ]; then
  cat $DEVFS_TEMP
  fi

  if [ "$command" = "2" ]; then
	 # echo "measure presure"
	cat $DEVFS_BMP085_PRESS
  fi


  RET=$?
if [ "$RET" = "0" ]; then
#wait for some time then take another reading
echo  "sleep"
	#sleep 1;
else
	echo "error"
	exit $?
fi
  i=`expr $i - 1`
done
exit 1

