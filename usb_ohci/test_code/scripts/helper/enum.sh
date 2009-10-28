#!/bin/sh

################################################################################
# enum.sh
# Author  : Diego Zavala Trujillo
# Date    : Janury 21, 2009
# Description: Script to enumerate the device or host. Ober the Mentor USB.
#	If an error happens, it will tell you where does it happens.
# Change Log:
#
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
			echo -e '\n\n\t Error, can not found '$3' on '$4'\n'
			sleep $DELAY1
			exit 1
		else
			echo -e '\n\n\t Error, found '$3' on '$4'\n'
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
	echo -e '\n\t '$ACT' \n'
	cat $PROC_INT | grep "Mode" | grep "$PERIPHERAL_MODE"
	detect_error $? 0 $PERIPHERAL_MODE $PROC_INT
	cat $PROC_INT | grep "$QUEUE_EMPTY"
	detect_error $? 0 $QUEUE_EMPTY $PROC_INT
	exit 0
	;;

connectHostRemote )
	echo -e '\n\t '$ACT' \n'
	(ssh root@$REMOTE_IP -i $SSH_PATH cat $ENUM_COMM) | grep "$ENUM_INFO"
	detect_error $? 0 $ENUM_INFO $ENUM_COMM-RemotePC
	exit 0
	;;

disconnectHost )
	echo -e '\n\t '$ACT' \n'
	cat $PROC_INT | grep "Mode" | grep "$PERIPHERAL_MODE"
	detect_error $? 0 $PERIPHERAL_MODE $PROC_INT
	cat $PROC_INT | grep "$QUEUE_EMPTY"
	cat $PROC_INT | grep "$QUEUE_EMPTY" && detect_error 1 1 $QUEUE_EMPTY $PROC_INT
	exit 0
	;;

disconnectHostRemote )
	echo -e '\n\t '$ACT' \n'
	(ssh root@$REMOTE_IP -i $SSH_PATH cat $ENUM_COMM) | grep "$ENUM_INFO" && detect_error 1 1 $ENUM_INFO $ENUM_COMM-RemotePC
	exit 0
	;;

# Connect a device on the OMAP
connectDevice )
	echo -e '\n\t '$ACT' \n'
	if [ "$USB_DRIVER" = "MENTOR" ] ; then
	echo 'F' > $PROC_INT
	cat $PROC_INT | grep "Mode" | grep "$HOST_MODE"
	detect_error $? 0 $HOST_MODE $PROC_INT
	fi
	cat $ENUM_COMM | grep "$ENUM_INFO"
	detect_error $? 0 $ENUM_INFO $PROC_INT
	exit 0
	;;

disconnectDevice )
	echo -e '\n\t '$ACT' \n'
	if [ $USB_DRIVER = "MENTOR" ] ; then
        echo 'F' > $PROC_INT
        fi
	cat $PROC_INT | grep 'Mode' | grep "$PERIPHERAL_MODE"
	detect_error $? 0 $PERIPHERAL_MODE $PROC_INT
	exit 0
	;;

checkinfoDevice )
	echo -e '\n\t '$ACT' \n'
	cat $ENUM_COMM | awk "/Bus=$BUS/, /$ENUM_INFO/" | awk "/Lev=$LEV/, /$ENUM_INFO/"  | grep "$ENUM_INFO"
	detect_error $? 0 $ENUM_INFO InfoDevice
	exit 0
	;;

checkspeedDevice )
	echo -e '\n\t '$ACT' \n'
        cat $ENUM_COMM | awk "/Bus=$BUS/, /$ENUM_INFO/" | awk "/Lev=$LEV/, /$ENUM_INFO/"  | grep "$SPEED"
        detect_error $? 0 $SPEED SpeedDevice
        exit 0
	;;

# Insert module on the Linux PC
insertRemoteModule )
	echo -e '\n\t '$ACT' \n'
	ssh root@$REMOTE_IP -i $SSH_PATH modprobe $MODULE
	detect_error $? 0 $PERIPHERAL_MODE $PROC_INT
	exit 0
	;;

removeRemoteModule )
	echo -e '\n\t '$ACT' \n'
	ssh root@$REMOTE_IP -i $SSH_PATH rmmod $MODULE
	exit 0
	;;

# Test USB control in out
testcio )
	echo -e '\n\t '$ACT' \n'
	(ssh root@$REMOTE_IP -i $SSH_PATH $REMOTE_PATH/test.sh control in out) > $LOGFILE-testusb-cio
	detect_error_testusb $? "control in out"
	exit 0
	;;

testc )
	echo -e '\n\t '$ACT' \n'
	if [ "$MODE" = "Host" ]; then
	echo -e '\n\n\t '$MESSAGE'  \n\n'
	cd ${TESTSCRIPT}/
	./test.sh $COMMAND > $LOGBASE/log.testControlOMAP
	cd -
	else
	(ssh root@$REMOTE_IP -i $SSH_PATH $REMOTE_PATH/test.sh control) > $LOGBASE/log.testControlLinuxPC
	detect_error_testusb $? "control"
	exit 0
	fi
	;;

testio )
	echo -e '\n\t '$ACT' \n'
	if [ "$MODE" = "Host" ]; then
	echo -e '\n\n\t '$MESSAGE'  \n\n'
	cd ${TESTSCRIPT}/
	./test.sh $COMMAND > $LOGBASE/log.testInOutOMAP
	cd -
	else
	(ssh root@$REMOTE_IP -i $SSH_PATH $REMOTE_PATH/test.sh in out) > $LOGBASE/log.testInOutlLinuxPC
	detect_error_testusb $? "in out"
	exit 0
	fi
	;;

esac
