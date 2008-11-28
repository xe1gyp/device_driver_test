#!/bin/sh

STATES=$1

ans=0
counter=0

put_on_state()
{
  echo
  echo "Power State $1"
  
	for i in $I2C_DEV
	do 
		CURRENT_ENTRY=`echo "SYSFS_POWER_ENTRY_$i"`
		echo -n $1 > $CURRENT_ENTRY
	done
	sleep 1  
}

set $STATES
case "$1" in 
    "DPM")
    
						if [ ! -d /sys/dpm ]; then
						{
							echo "FATAL: /sys/dpm not found"
						  exit 1
						}
						else
	            while [ $counter -lt $2 ]
	            do
	              counter=`expr $counter + 1`
	              echo -e "$SCALE_OPTION\n$QUIT_OPTION"|$DPM_SCRIPT;
	              sleep 2
	            done
          	fi
         ;; 
    "SLEEP")

						if [ ! -e $SYSFS_SYSTEM_POWER_ENTRY ]; then
						{
							echo "FATAL: $SYSFS_SYSTEM_POWER_ENTRY not found"
						  exit 1
						}
						fi    
    
		        while [ $counter -lt $2 ]
            do
              counter=`expr $counter + 1`
              if [ -z "$STRESS" ]; then
                echo -n $SYSFS_SYSTEM_POWER_SLEEP > $SYSFS_SYSTEM_POWER_ENTRY;
              fi
              sleep 2
            done
         ;;
    *) 

						if [ ! -e $SYSFS_POWER_ENTRY ]; then
						{
							echo "FATAL: $SYSFS_POWER_ENTRY not found"
						  exit 1
						}
						fi    
            for st in $STATES
            do
              put_on_state $st
            done
         ;;
esac

