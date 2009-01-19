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
			sleep $DELAY2
			exit 1
		else
			echo -e '\n\n\t Error, found '$3' on '$4'\n'
			sleep $DELAY2
			exit 1
		fi
	fi
}

# ACT is the variable where you say wich case to use, when connecting a device,
# when disconnecting a device.

case $ACT in

# Connect a Host on the OMAP
connectHost )
	echo -e '\t Checking enumeration \n'
	#echo 'F' > $PROC_INT
	cat $PROC_INT | grep "Mode" | grep "$PERIPHERAL_MODE"
	detect_error $? 0 $PERIPHERAL_MODE $PROC_INT
	cat $PROC_INT | grep "$QUEUE_EMPTY"
	cat $PROC_INT | grep "$QUEUE_EMPTY"
	detect_error $? 0 $QUEUE_EMPTY $PROC_INT
	exit 0
	;;

connectHostRemote )
	echo -e '\t Checking enumeration on Linux PC'
	(ssh root@$REMOTE_IP -i $SSH_PATH cat $ENUM_COMM) | grep "$ENUM_INFO"
	detect_error $? 0 $ENUM_INFO $ENUM_COMM-RemotePC
	exit 0
	;;

disconnectHost )
	echo -e '\t Checking des enumeration \n'
	#echo 'F' > $PROC_INT
	cat $PROC_INT | grep "Mode" | grep "$PERIPHERAL_MODE"
	detect_error $? 0 $PERIPHERAL_MODE $PROC_INT
	cat $PROC_INT | grep "$QUEUE_EMPTY"
	cat $PROC_INT | grep "$QUEUE_EMPTY" && detect_error 1 1 $QUEUE_EMPTY $PROC_INT
	exit 0
	;;


# Connect a device on the OMAP
connectDevice )
	echo -e '\t Checking enumeration'
	echo 'F' > $PROC_INT
	cat $PROC_INT | grep "Mode" | grep "$HOST_MODE"
	detect_error $? 0 $HOST_MODE $PROC_INT
	cat $ENUM_COMM | grep "$ENUM_INFO"
	detect_error $? 0 $ENUM_INFO $PROC_INT
	exit 0
	;;

disconnectDevice )
	echo -e '\t Checking des enumeration'
	echo 'F' > $PROC_INT
	cat $PROC_INT | grep 'Mode' | grep "$PERIPHERAL_MODE"
	detect_error $? 0 $PERIPHERAL_MODE $PROC_INT
	exit 0
	;;

esac
