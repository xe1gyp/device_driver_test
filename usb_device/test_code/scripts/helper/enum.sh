#!/bin/sh

################################################################################
# enum.sh
# Author  : Erandi Chavolla Ugarte
# Date    : October 23th, 2009
# Description: Script to enumerate the device or host. Over the Mentor USB.
################################################################################

#1   echo 'F' > /proc/driver/musb_hdrc
#    cat /proc/driver/musb_hdrc | grep 'Mode' | grep 'Host'
#2   echo 'F' > /proc/driver/musb_hdrc
#    cat /proc/driver/musb_hdrc | grep 'Mode' | grep 'Peripheral'
#1   echo -n mem > /sys/power/state
#0  insmod /testsuites/usb_device/mods/g_file_storage.ko file=/testsuites/usb_device/scripts/misc/vfat-4M_12xand18x stall=0

set -x
ACT=$1

# Will see if ther is any error when the device is conected or not.
# $1 detected or not
# $2 found or not found
# $3 string that is looking for
# $4 place where is looking

detect_error ()
{
	if [ $1 == 1 ];	then
		if [ $2 == 0 ]; then
			echo -e "\n\n\t Error 2 - Can not found $3 on $4 \n"
			sleep $DELAY1
			exit 1
		else
			echo -e "\n\n\t Error 3 - Found $3 on $4 \n"
			sleep $DELAY1
			exit 1
		fi
	fi
}

detect_error_testusb ()
{
	if [ $1 == 1 ];	then
		echo -e '\n\n\t The testusb '$2' can not be completed \n'
		sleep $DELAY2
		exit 1
	fi
}



# ACT is the variable where you say wich case to use, when connecting a device,
# when disconnecting a device.

case $ACT in

# Connect a Host on the OMAP
connectHost )
	echo -e "\n\t ${ACT} \n"
	cat $PROC_INT | grep "Mode" | grep "$PERIPHERAL_MODE"
	cat $PROC_INT | grep "$QUEUE_EMPTY"
	exit 0
	;;

connectHostRemote )
	echo -e "\n\t ${ACT} \n"
	ssh root@$REMOTE_IP -i $SSH_PATH lsmod | grep usbserial
  	exit 0
	;;
	
connectHostRemoteSusp )
	echo -e "\n\t ${ACT} \n"
	(ssh root@$REMOTE_IP -i $SSH_PATH cat $ENUM_COMM) | grep "$ENUM_INFO"
	exit 0
	;;

connectProcRemote )
	echo -e "\n\t ${ACT} \n"
	(ssh root@$REMOTE_IP -i $SSH_PATH cat $GADGET_PROC) | grep "$ENUM_INFO"
	exit 0
	;;

connectNetworkInterface )
	echo -e "\n\t ${ACT} \n"
	(ssh root@$REMOTE_IP -i $SSH_PATH cat $NET_INTERFACE)
  	exit 0
	;;
disconnectHost )
	echo -e "\n\t ${ACT} \n"
	cat $PROC_INT | grep "Mode" | grep "$PERIPHERAL_MODE"
	cat $PROC_INT | grep "$QUEUE_EMPTY"
	cat $PROC_INT | grep "$QUEUE_EMPTY" && detect_error 1 1 $QUEUE_EMPTY $PROC_INT
	exit 0
	;;

disconnectHostRemote )
	echo -e "\n\t ${ACT} \n"
	(ssh root@$REMOTE_IP -i $SSH_PATH cat $ENUM_COMM) | grep "$ENUM_INFO" && detect_error 1 1 $ENUM_INFO $ENUM_COMM-RemotePC
	exit 0
	;;

# Connect a device on the OMAP
connectDevice )
	echo -e "\n\t ${ACT} \n"
	if [ "$USB_DRIVER" = "MENTOR" ] ; then
	echo 'F' > $PROC_INT
	cat $PROC_INT | grep "Mode" | grep "$HOST_MODE"
	fi
	cat $ENUM_COMM | grep "$ENUM_INFO"
	exit 0
	;;

disconnectDevice )
	echo -e "\n\t ${ACT} \n"
	if [ $USB_DRIVER = "MENTOR" ] ; then
        echo 'F' > $PROC_INT
        fi
	cat $PROC_INT | grep 'Mode' | grep "$PERIPHERAL_MODE"
	exit 0
	;;

checkinfoDevice )
	echo -e "\n\t ${ACT} \n"
	cat $ENUM_COMM | awk "/Bus=$BUS/, /$ENUM_INFO/" | awk "/Lev=$LEV/, /$ENUM_INFO/"  | grep "$ENUM_INFO"
	exit 0
	;;

checkspeedDevice )
	echo -e "\n\t ${ACT} \n"
	cat $ENUM_COMM | awk "/Bus=$BUS/, /$ENUM_INFO/" | awk "/Lev=$LEV/, /$ENUM_INFO/"  | grep "$SPEED"
	exit 0
	;;

# Insert module on the Linux PC
insertRemoteModule )
	echo -e "\n\t ${ACT} \n"
	ssh root@$REMOTE_IP -i $SSH_PATH modprobe $MODULE 
	ssh root@$REMOTE_IP -i $SSH_PATH lsmod | grep $MODULE
 	exit 0
	;;

removeRemoteModule )
	echo -e "\n\t ${ACT} \n"
	ssh root@$REMOTE_IP -i $SSH_PATH rmmod $MODULE
	ssh root@$REMOTE_IP -i $SSH_PATH lsmod | grep $MODULE
	exit 0
	;;

createRemoteNode )
	echo -e "\n\t ${ACT} \n"
	;;

# IRQ AFFINITY LINUX 

irqaffinityL )
	
	echo -e "\n\t ${ACT} \n"
	echo -e "\nInfo: IRQ AFFINITY\n"

test -f /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity

if [ $? -eq 1 ]
then
	echo "Error: Cannot set affinity for irq $LOCAL_IRQ_NUMBER!"
	
fi
		
	ssh root@$REMOTE_IP -i $SSH_PATH cp  $REMOTE_PATH/$GADGET_ST_FILE $REMOTE_PATH/mass_storage &&  sleep $DELAY2  &
	
while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do

		echo -e "\nInfo: PID $LOCAL_COMMAND_PID | Irq Number $LOCAL_IRQ_NUMBER | Count $count of $LOCAL_EXECUTION_TIMES"
		if [ ! -d "/proc/$LOCAL_COMMAND_PID" ]
		then
			break
		fi

		rem=$(( $count % 2 ))

		if [ $rem -eq 0 ]
		then

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 1 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p1" -lt "$final_value_p1" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 1"
				
			fi

		else

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 2 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p2" -lt "$final_value_p2" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 2"
				
			fi

		fi

		count=`expr $count + 1`
		sleep $LOCAL_TIME_TO_WAIT

	done

	echo -e "Info: Waiting for it to finish..."
	wait
	echo -e "Info: Done!\n"
exit 0
	;;


# IRQ AFFINITY OMAP

irqaffinityO )
	
	echo -e "\n\t ${ACT} \n"
	echo -e "\nInfo: IRQ AFFINITY\n"

test -f /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity

if [ $? -eq 1 ]
then
	echo "Error: Cannot set affinity for irq $LOCAL_IRQ_NUMBER!"
fi
		
	ssh root@$REMOTE_IP -i $SSH_PATH cp  $REMOTE_PATH/mass_storage/$GADGET_ST_FILE  $REMOTE_PATH/  &&  sleep $DELAY2  &
	
while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do

		echo -e "\nInfo: PID $LOCAL_COMMAND_PID | Irq Number $LOCAL_IRQ_NUMBER | Count $count of $LOCAL_EXECUTION_TIMES"
		if [ ! -d "/proc/$LOCAL_COMMAND_PID" ]
		then
			break
		fi

		rem=$(( $count % 2 ))

		if [ $rem -eq 0 ]
		then

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 1 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p1" -lt "$final_value_p1" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 1"
			fi

		else

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 2 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p2" -lt "$final_value_p2" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 2"
			fi

		fi

		count=`expr $count + 1`
		sleep $LOCAL_TIME_TO_WAIT

	done

	echo -e "Info: Waiting for it to finish..."
	wait
	echo -e "Info: Done!\n"
exit 0
	;;

# CPU AFFINITY LINUX PC

cpuaffinityL )
	
	echo -e "\n\t ${ACT} \n"
	echo -e "\nInfo: CPU AFFINITY\n"
	
	ssh root@$REMOTE_IP -i $SSH_PATH cp  $REMOTE_PATH/$GADGET_ST_FILE $REMOTE_PATH/mass_storage &&  sleep $DELAY2  &
		
	LOCAL_COMMAND_PID=`echo $!`	
	
	while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do
		echo -e "\nInfo: PID $LOCAL_COMMAND_PID | Processor $processor | Count $count of $LOCAL_EXECUTION_TIMES"

		if [ ! -d "/proc/$LOCAL_COMMAND_PID" ]
		then
			break
		fi
		rem=$(( $count % 2 ))

		if [ $rem -eq 0 ]
		then
			processor=1
			$UTILS_DIR_BIN/taskset -p $processor $LOCAL_COMMAND_PID
		else
			processor=2
			$UTILS_DIR_BIN/taskset -p $processor $LOCAL_COMMAND_PID
		fi

		if [ $? -ne 0 ]
		then
			echo -e "Error: Could not set cpu affinity for processor $processor!"

		fi

		count=`expr $count + 1`
		sleep $LOCAL_TIME_TO_WAIT

	done

	echo -e "Info: Waiting for it to finish..."
	wait
	echo -e "Info: Done!\n"
exit 0
	;;
	
# CPU AFFINITY OMAP

cpuaffinityO )
	
	echo -e "\n\t ${ACT} \n"
	ssh root@$REMOTE_IP -i $SSH_PATH cp  $REMOTE_PATH/mass_storage/$GADGET_ST_FILE  $REMOTE_PATH/  &&  sleep $DELAY2  &
		
	LOCAL_COMMAND_PID=`echo $!`	
	
	while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do
		echo -e "\nInfo: PID $LOCAL_COMMAND_PID | Processor $processor | Count $count of $LOCAL_EXECUTION_TIMES"

		if [ ! -d "/proc/$LOCAL_COMMAND_PID" ]
		then
			break
		fi
		rem=$(( $count % 2 ))

		if [ $rem -eq 0 ]
		then
			processor=1
			$UTILS_DIR_BIN/taskset -p $processor $LOCAL_COMMAND_PID
		else
			processor=2
			$UTILS_DIR_BIN/taskset -p $processor $LOCAL_COMMAND_PID
		fi

		if [ $? -ne 0 ]
		then
			echo -e "Error: Could not set cpu affinity for processor $processor!"

		fi

		count=`expr $count + 1`
		sleep $LOCAL_TIME_TO_WAIT

	done

	echo -e "Info: Waiting for it to finish..."
	wait
	echo -e "Info: Done!\n"
exit 0
	;;
	
	
# Serial Process

irqserialonlinux )

	echo -e "\n\t ${ACT} \n"
	echo -e "\nInfo: IRQ AFFINITY\n"

test -f /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity

if [ $? -eq 1 ]
then
	echo "Error: Cannot set affinity for irq $LOCAL_IRQ_NUMBER!"
fi
	(cat $MISC_PATH/file2send-OMAP > /dev/ttyGS0)&  sleep $DELAYMAX	
	
while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do

		echo -e "\nInfo: PID $LOCAL_COMMAND_PID | Irq Number $LOCAL_IRQ_NUMBER | Count $count of $LOCAL_EXECUTION_TIMES"
		if [ ! -d "/proc/$LOCAL_COMMAND_PID" ]
		then
			break
		fi

		rem=$(( $count % 2 ))

		if [ $rem -eq 0 ]
		then

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 1 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p1" -lt "$final_value_p1" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 1"
			fi

		else

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 2 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p2" -lt "$final_value_p2" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 2"
			fi

		fi

		count=`expr $count + 1`
		sleep $LOCAL_TIME_TO_WAIT

	done

	exit 0
	;;


irqserialonOMAP )

	echo -e "\n\t ${ACT} \n"
	echo -e "\nInfo: IRQ AFFINITY\n"

test -f /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity

if [ $? -eq 1 ]
then
	echo "Error: Cannot set affinity for irq $LOCAL_IRQ_NUMBER!"
fi
	((cat /dev/ttyGS0)  &  (sleep $DELAY2 && ssh root@$REMOTE_IP -i $SSH_PATH  cat $REMOTE_PATH/file2send-PC) > $USBDEVICE_DIR_LOG/log.serial-PCSide)& sleep $DELAYMAX  
		
while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do

		echo -e "\nInfo: PID $LOCAL_COMMAND_PID | Irq Number $LOCAL_IRQ_NUMBER | Count $count of $LOCAL_EXECUTION_TIMES"
		if [ ! -d "/proc/$LOCAL_COMMAND_PID" ]
		then
			break
		fi

		rem=$(( $count % 2 ))

		if [ $rem -eq 0 ]
		then

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 1 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p1" -lt "$final_value_p1" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 1"
			fi

		else

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 2 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p2" -lt "$final_value_p2" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 2"
			fi

		fi

		count=`expr $count + 1`
		sleep $LOCAL_TIME_TO_WAIT

	done

	exit 0
	;;
	
# Ethernet 

etherneto )

	echo -e "\n\t ${ACT} \n"
	echo -e "\nInfo: IRQ AFFINITY\n"

test -f /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity

if [ $? -eq 1 ]
then
	echo "Error: Cannot set affinity for irq $LOCAL_IRQ_NUMBER!"
fi
	ping -c 5 $HOST_IP > $USBDEVICE_DIR_LOG/log.ethernetOMAP
		
while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do

		echo -e "\nInfo: PID $LOCAL_COMMAND_PID | Irq Number $LOCAL_IRQ_NUMBER | Count $count of $LOCAL_EXECUTION_TIMES"
		if [ ! -d "/proc/$LOCAL_COMMAND_PID" ]
		then
			break
		fi

		rem=$(( $count % 2 ))

		if [ $rem -eq 0 ]
		then

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 1 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p1" -lt "$final_value_p1" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 1"
			fi

		else

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 2 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p2" -lt "$final_value_p2" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 2"
			fi

		fi

		count=`expr $count + 1`
		sleep $LOCAL_TIME_TO_WAIT

	done

	exit 0
	;;



ethernetl )

	echo -e "\n\t ${ACT} \n"
	echo -e "\nInfo: IRQ AFFINITY\n"

test -f /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity

if [ $? -eq 1 ]
then
	echo "Error: Cannot set affinity for irq $LOCAL_IRQ_NUMBER!"
fi
	((ssh root@$REMOTE_IP -i $SSH_PATH ping -c 5 $GADGET_IP) > $USBDEVICE_DIR_LOG/log.ethernetLinuxPC)& sleep $DELAYMAX
		
while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do

		echo -e "\nInfo: PID $LOCAL_COMMAND_PID | Irq Number $LOCAL_IRQ_NUMBER | Count $count of $LOCAL_EXECUTION_TIMES"
		if [ ! -d "/proc/$LOCAL_COMMAND_PID" ]
		then
			break
		fi

		rem=$(( $count % 2 ))

		if [ $rem -eq 0 ]
		then

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 1 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p1" -lt "$final_value_p1" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 1"
			fi

		else

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 2 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p2" -lt "$final_value_p2" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 2"
			fi

		fi

		count=`expr $count + 1`
		sleep $LOCAL_TIME_TO_WAIT

	done

	exit 0
	;;
	
# Test USB control in out

testcontrol )
	echo -e "\n\t ${ACT} \n"
	
	(ssh root@$REMOTE_IP -i $SSH_PATH $REMOTE_PATH/test.sh control > $USBDEVICE_DIR_LOG/log.testControlLinuxPC)& sleep $DELAYMAX 
	detect_error_testusb $? "control"
	exit 0
	;;
	
testc )

	echo -e "\n\t ${ACT} \n"
	echo -e "\nInfo: IRQ AFFINITY\n"

test -f /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity

if [ $? -eq 1 ]
then
	echo "Error: Cannot set affinity for irq $LOCAL_IRQ_NUMBER!"
fi
	(ssh root@$REMOTE_IP -i $SSH_PATH $REMOTE_PATH/test.sh control > $USBDEVICE_DIR_LOG/log.testControlLinuxPC)& sleep $DELAYMAX 		

	
while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do

		echo -e "\nInfo: PID $LOCAL_COMMAND_PID | Irq Number $LOCAL_IRQ_NUMBER | Count $count of $LOCAL_EXECUTION_TIMES"
		if [ ! -d "/proc/$LOCAL_COMMAND_PID" ]
		then
			break
		fi

		rem=$(( $count % 2 ))

		if [ $rem -eq 0 ]
		then

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 1 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p1" -lt "$final_value_p1" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 1"
			fi

		else

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 2 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p2" -lt "$final_value_p2" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 2"
			fi

		fi

		count=`expr $count + 1`
		sleep $LOCAL_TIME_TO_WAIT

	done

	echo -e "Info: Waiting for it to finish..."
	wait
	echo -e "Info: Done!\n"

	cat $USBDEVICE_DIR_LOG/log.testControlLinuxPC
	sleep $DELAY5
	exit 0
	;;

testio )

	echo -e "\n\t ${ACT} \n"
	echo -e "\nInfo: IRQ AFFINITY\n"

test -f /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity

if [ $? -eq 1 ]
then
	echo "Error: Cannot set affinity for irq $LOCAL_IRQ_NUMBER!"
fi
	(ssh root@$REMOTE_IP -i $SSH_PATH $REMOTE_PATH/test.sh in out > $USBDEVICE_DIR_LOG/log.testInOutlLinuxPC)& sleep $DELAYMAX  		

	
while [ $count -le $LOCAL_EXECUTION_TIMES ]
	do

		echo -e "\nInfo: PID $LOCAL_COMMAND_PID | Irq Number $LOCAL_IRQ_NUMBER | Count $count of $LOCAL_EXECUTION_TIMES"
		if [ ! -d "/proc/$LOCAL_COMMAND_PID" ]
		then
			break
		fi

		rem=$(( $count % 2 ))

		if [ $rem -eq 0 ]
		then

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 1 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p1" -lt "$final_value_p1" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 1"
				
			fi

		else

			initial_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			initial_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo 2 > /proc/irq/$LOCAL_IRQ_NUMBER/smp_affinity
			sleep $LOCAL_TIME_TO_WAIT
			final_value_p1=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $2}'`
			final_value_p2=`cat /proc/interrupts | grep $LOCAL_IRQ_NUMBER | awk '{print $3}'`
			echo $initial_value_p1 $final_value_p1 $initial_value_p2 $final_value_p2
			if [ "$initial_value_p2" -lt "$final_value_p2" ]
			then
				continue
			else
				echo "Error: Number of interrupts were not increased in Processor 2"
			fi

		fi

		count=`expr $count + 1`
		sleep $LOCAL_TIME_TO_WAIT

	done

	echo -e "Info: Waiting for it to finish..."
	wait
	echo -e "Info: Done!\n"

	cat $USBDEVICE_DIR_LOG/log.testInOutlLinuxPCC
	sleep $DELAY5
	exit 0
	;;


createvFat )
	echo -e "\n\t ${ACT} \n"
	dd if=/dev/zero of=$VFAT_FILE bs=1M count=4
	# echo -n "x\n s\n" | fdisk $VFAT_FILE
	mkfs.vfat $VFAT_FILE
	;;

dellvFat )
	echo -e "\n\t ${ACT} \n"
	rm -rf $VFAT_FILE
	;;

creatFileRemote )
	echo -e "\n\t ${ACT} \n"
	ssh root@$REMOTE_IP -i $SSH_PATH dd if=${DD_IF} of=${REMOTE_PATH}/${GADGET_ST_FILE} bs=1M count=1
	;;
	
dellFileRemote )
	echo -e "\n\t ${ACT} \n"
	ssh root@$REMOTE_IP -i $SSH_PATH rm -rf ${REMOTE_PATH}/$GADGET_ST_FILE
	;;

mountRemote )
	echo -e "\n\t ${ACT} \n"
	ssh root@$REMOTE_IP -i $SSH_PATH mkdir $REMOTE_PATH/mass_storage
	ssh root@$REMOTE_IP -i $SSH_PATH df -h
	ssh root@$REMOTE_IP -i $SSH_PATH sleep $DELAY2
	ssh root@$REMOTE_IP -i $SSH_PATH ls /dev/sd*
	ssh root@$REMOTE_IP -i $SSH_PATH 'echo -e "p\n n\n p\n 1\n \n +128\n p\n w\n" | fdisk /dev/sdb'
	ssh root@$REMOTE_IP -i $SSH_PATH sleep $DELAY2
	ssh root@$REMOTE_IP -i $SSH_PATH mke2fs ${USB_DEVFS_PARTITION}
	ssh root@$REMOTE_IP -i $SSH_PATH mount ${USB_DEVFS_PARTITION} $REMOTE_PATH/mass_storage
	ssh root@$REMOTE_IP -i $SSH_PATH df -h
	ssh root@$REMOTE_IP -i $SSH_PATH sleep $DELAY2
	if [ $? = "0" ]; then
		echo -e "\n\n\t The device was mounted with out any problem \n\n"
		exit 0
	else
		echo -e "\n\n\t The device was not mounted, please check if is detected on /dev/sd*"
		exit 1
	fi
	;;

umountRemote )
	echo -e "\n\t ${ACT} \n"
	ssh root@$REMOTE_IP -i $SSH_PATH umount $REMOTE_PATH/mass_storage
	ssh root@$REMOTE_IP -i $SSH_PATH rm -rf $REMOTE_PATH/mass_storage
	ssh root@$REMOTE_IP -i $SSH_PATH df -h
	if [ $? = "0" ]; then
		echo -e "\n\n\t The device was unmounted with out any problem. \n\n"
		exit 0
	else
		echo -e "\n\n\t The device was not unmounted. \n\n"
		exit 1
	fi
	;;



esac
