#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_DRIVER=$1
DEVICE_TYPE=$2
MAX_ITER=100
COUNTER=0


while [ $COUNTER -lt 100 ]
do

	echo Iteration : $COUNTER
    	COUNTER=`expr $COUNTER + 1`	
	if [ "$LOCAL_DRIVER" = "musb" ]; then
		echo "USB Bind/Unbind test MUSB"	
		if [ "$DEVICE_TYPE" = "storage" ]; then
			echo " USB Bus $USB_BUS"	
			for i in /sys/bus/usb/drivers/usb-storage/1-1.*; do
				echo " path of USB device $i"
				echo ${i: -9}
				USB_PORTNO=`echo ${i: -9}`
				echo " usb port no $USB_PORTNO"
				echo " usb path $i/$USB_PORTNO/"
				echo "Unbind the device"
				echo $USB_PORTNO > /sys/bus/usb/drivers/usb-storage/unbind && sleep 2
				USB_UNBIND=`cat /proc/bus/usb/devices | grep stor | grep none`
				echo $USB_UNBIND
				if [ "$USB_UNBIND" = "" ]; then
					exit 1
				fi
				echo "Bind the device"
				echo $USB_PORTNO > /sys/bus/usb/drivers/usb-storage/bind && sleep 2
				USB_BIND=`cat /proc/bus/usb/devices | grep stor | grep usb-storage`
				echo $USB_BIND
				if [ "$USB_BIND" = "" ]; then
					exit 1
				fi
		
			done
		fi
		if [ "$DEVICE_TYPE" = "keyspan" ]; then
			for i in /sys/bus/usb/drivers/keyspan/1-1.*; do
				echo " path of USB device $i"
				echo ${i: -9}
				USB_PORTNO=`echo ${i: -9}`
				echo " usb port no $USB_PORTNO"
				echo " usb path $i/$USB_PORTNO/"
				cat /proc/bus/usb/devices
				echo "Unbind the device"
				echo $USB_PORTNO > /sys/bus/usb/drivers/keyspan/unbind && sleep 2
			
				USB_UNBIND=`cat /proc/bus/usb/devices | grep none`
				echo $USB_UNBIND
				if [ "$USB_UNBIND" = "" ]; then
					echo "UnBind failed"				
					exit 1
				fi
				echo "Bind the device"
				echo $USB_PORTNO > /sys/bus/usb/drivers/keyspan/bind && sleep 2
				USB_BIND=`cat /proc/bus/usb/devices | grep keyspan`
				echo $USB_BIND
				if [ "$USB_BIND" = "" ]; then
					echo "Bind failed"				
					exit 1
				fi
		
			done
		fi
	
		if [ "$DEVICE_TYPE" = "ether" ]; then
			ls /sys/bus/usb/drivers/asix
			for i in /sys/bus/usb/drivers/asix/1-1.*; do
				echo " path of USB device $i"
				echo ${i: -9}
				USB_PORTNO=`echo ${i: -9}`
				echo " usb port no $USB_PORTNO"
				echo " usb path $i/$USB_PORTNO/"
				echo "Unbind the device"
				ls /sys/bus/usb/drivers/asix
				echo $USB_PORTNO > /sys/bus/usb/drivers/asix/unbind && sleep 2
				USB_UNBIND=`cat /proc/bus/usb/devices | grep ifc | grep asix`
				echo $USB_UNBIND
				if [ "$USB_UNBIND" != "" ]; then
					exit 1
				fi
				echo "Bind the device"
				echo $USB_PORTNO > /sys/bus/usb/drivers/asix/bind && sleep 2
				USB_BIND=`cat /proc/bus/usb/devices | grep ifc | grep asix`
				echo $USB_BIND
				if [ "$USB_BIND" = "" ]; then
					exit 1
				fi
		
			done
		fi
	fi

	if [ "$LOCAL_DRIVER" = "ehci" ]; then
	
		echo "USB Bind/Unbind test EHCI"
		USB_NO=`grep -s Lev=01 /proc/bus/usb/devices | grep -s Bus=02`
		echo " USB no $USB_NO"
		USB_BUS=2
		if [ "$USB_NO" = "" ]; then
			USB_BUS=3
		fi
		echo $LOCAL_DRIVER $DEVICE_TYPE
		if [ "$DEVICE_TYPE" = "storage" ]; then
			echo " USB Bus $USB_BUS"	
			for i in /sys/bus/usb/drivers/usb-storage/$USB_BUS-1.*; do
				echo " path of USB device $i"
				echo ${i: -9}
				USB_PORTNO=`echo ${i: -9}`
				echo " usb port no $USB_PORTNO"
				echo " usb path $i/$USB_PORTNO/"
				echo "Unbind the device"
				echo $USB_PORTNO > /sys/bus/usb/drivers/usb-storage/unbind && sleep 2
				USB_UNBIND=`cat /proc/bus/usb/devices | grep stor | grep none`
				echo $USB_UNBIND
				if [ "$USB_UNBIND" = "" ]; then
					exit 1
				fi
				echo "Bind the device"
				echo $USB_PORTNO > /sys/bus/usb/drivers/usb-storage/bind && sleep 2
				USB_BIND=`cat /proc/bus/usb/devices | grep stor | grep usb-storage`
				echo $USB_BIND
				if [ "$USB_BIND" = "" ]; then
					exit 1
				fi
		
			done
		fi
		if [ "$DEVICE_TYPE" = "keyspan" ]; then
			for i in /sys/bus/usb/drivers/keyspan/$USB_BUS-1.*; do
				echo " path of USB device $i"
				echo ${i: -9}
				USB_PORTNO=`echo ${i: -9}`
				echo " usb port no $USB_PORTNO"
				echo " usb path $i/$USB_PORTNO/"
				cat /proc/bus/usb/devices
				echo "Unbind the device"
				echo $USB_PORTNO > /sys/bus/usb/drivers/keyspan/unbind && sleep 2
			
				USB_UNBIND=`cat /proc/bus/usb/devices | grep none`
				echo $USB_UNBIND
				if [ "$USB_UNBIND" = "" ]; then
					echo "UnBind failed"				
					exit 1
				fi
				echo "Bind the device"
				echo $USB_PORTNO > /sys/bus/usb/drivers/keyspan/bind && sleep 2
				USB_BIND=`cat /proc/bus/usb/devices | grep keyspan`
				echo $USB_BIND
				if [ "$USB_BIND" = "" ]; then
					echo "Bind failed"				
					exit 1
				fi
		
			done
		fi
	
		if [ "$DEVICE_TYPE" = "ether" ]; then
			for i in /sys/bus/usb/drivers/asix/$USB_BUS-1.*; do
				echo " path of USB device $i"
				echo ${i: -9}
				USB_PORTNO=`echo ${i: -9}`
				echo " usb port no $USB_PORTNO"
				echo " usb path $i/$USB_PORTNO/"
				echo "Unbind the device"
				ls /sys/bus/usb/drivers/asix
				echo $USB_PORTNO > /sys/bus/usb/drivers/asix/unbind && sleep 2
				USB_UNBIND=`cat /proc/bus/usb/devices | grep ifc | grep asix`
				echo $USB_UNBIND
				if [ "$USB_UNBIND" = "" ]; then
					exit 1
				fi
				echo "Bind the device"
				echo $USB_PORTNO > /sys/bus/usb/drivers/asix/bind && sleep 2
				USB_BIND=`cat /proc/bus/usb/devices | grep ifc | grep asix`
				echo $USB_BIND
				if [ "$USB_BIND" != "" ]; then
					exit 1
				fi
		
			done
		fi
	fi


	if [ "$LOCAL_DRIVER" = "ohci" ]; then
		echo "USB Bind/Unbind test OHCI"	
		USB_NO=`grep -s Lev=01 /proc/bus/usb/devices | grep -s Bus=02`
		echo " USB no $USB_NO"
		USB_BUS=2
		if [ "$USB_NO" = "" ]; then
			USB_BUS=3
		fi
		if [ "$DEVICE_TYPE" = "storage" ]; then
			echo " USB Bus $USB_BUS"	
			for i in /sys/bus/usb/drivers/usb-storage/$USB_BUS-2.*; do
				echo " path of USB device $i"
				echo ${i: -9}
				USB_PORTNO=`echo ${i: -9}`
				echo " usb port no $USB_PORTNO"
				echo " usb path $i/$USB_PORTNO/"
				echo "Unbind the device"
				echo $USB_PORTNO > /sys/bus/usb/drivers/usb-storage/unbind && sleep 2
				USB_UNBIND=`cat /proc/bus/usb/devices | grep stor | grep none`
				echo $USB_UNBIND
				if [ "$USB_UNBIND" = "" ]; then
					exit 1
				fi
				echo "Bind the device"
				echo $USB_PORTNO > /sys/bus/usb/drivers/usb-storage/bind && sleep 2
				USB_BIND=`cat /proc/bus/usb/devices | grep stor | grep usb-storage`
				echo $USB_BIND
				if [ "$USB_BIND" = "" ]; then
					exit 1
				fi
		
			done
		fi
		if [ "$DEVICE_TYPE" = "keyspan" ]; then
			for i in /sys/bus/usb/drivers/keyspan/$USB_BUS-2.*; do
				echo " path of USB device $i"
				echo ${i: -9}
				USB_PORTNO=`echo ${i: -9}`
				echo " usb port no $USB_PORTNO"
				echo " usb path $i/$USB_PORTNO/"
				cat /proc/bus/usb/devices
				echo "Unbind the device"
				echo $USB_PORTNO > /sys/bus/usb/drivers/keyspan/unbind && sleep 2
			
				USB_UNBIND=`cat /proc/bus/usb/devices | grep none`
				echo $USB_UNBIND
				if [ "$USB_UNBIND" = "" ]; then
					echo "UnBind failed"				
					exit 1
				fi
				echo "Bind the device"
				echo $USB_PORTNO > /sys/bus/usb/drivers/keyspan/bind && sleep 2
				USB_BIND=`cat /proc/bus/usb/devices | grep keyspan`
				echo $USB_BIND
				if [ "$USB_BIND" = "" ]; then
					echo "Bind failed"				
					exit 1
				fi
		
			done
		fi
	
		if [ "$DEVICE_TYPE" = "ether" ]; then
			ls /sys/bus/usb/drivers/asix
			for i in /sys/bus/usb/drivers/asix/$USB_BUS-2.*; do
				echo " path of USB device $i"
				echo ${i: -9}
				USB_PORTNO=`echo ${i: -9}`
				echo " usb port no $USB_PORTNO"
				echo " usb path $i/$USB_PORTNO/"
				echo "Unbind the device"
				ls /sys/bus/usb/drivers/asix
				echo $USB_PORTNO > /sys/bus/usb/drivers/asix/unbind && sleep 2
				USB_UNBIND=`cat /proc/bus/usb/devices | grep ifc | grep asix`
				echo $USB_UNBIND
				if [ "$USB_UNBIND" != "" ]; then
					exit 1
				fi
				echo "Bind the device"
				echo $USB_PORTNO > /sys/bus/usb/drivers/asix/bind && sleep 2
				USB_BIND=`cat /proc/bus/usb/devices | grep ifc | grep asix`
				echo $USB_BIND
				if [ "$USB_BIND" = "" ]; then
					exit 1
				fi
		
			done
		fi
	fi
done





	
