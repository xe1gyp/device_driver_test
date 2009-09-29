#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_COMMAND=$1
LOCAL_DRIVER=$2

# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_COMMAND" = "create" ]; then
	
	if [ "$LOCAL_DRIVER" = "mentor" ]; then
	
		echo "Inserting the module"
		insmod $UTILSMODULES/g_zero.ko
		lsmod | grep  g_zero
		sleep 10
		echo "otg" > $USBHOST_ENUMERATION
		cat /proc/bus/usb/devices
		
	elif [ "$LOCAL_DRIVER" = "ehci" ]; then
		
		echo "Inserting the module"
		insmod $UTILSMODULES/ehci-hcd.ko
		echo "Checking and interact with sysfs ehci entries"
		ls $SYSFS_EHCI_OMAP/usb1
	
		if [ $? = 0 ]; then
				echo on >  $SYSFS_EHCI_OMAP/usb1/power/level
		else 
			ls $SYSFS_EHCI_OMAP/usb2
			if [ $? = 0 ]; then
				echo on >  $SYSFS_EHCI_OMAP/usb2/power/level
			fi
		fi
	lsmod | grep  ehci_hcd
	sleep 10
	cat /proc/bus/usb/devices
	
	
	elif [ "$LOCAL_DRIVER" = "ohci" ]; then

		echo "Inserting the module"
		insmod $UTILSMODULES/ohci-hcd.ko
		echo "Checking and interact with sysfs ochi entries"
		ls $SYSFS_OHCI_OMAP/usb1
    		
		if [ $? = 0 ]; then
				echo on >  $SYSFS_OHCI_OMAP/usb1/power/level
		else 
			ls $SYSFS_OHCI_OMAP/usb2
      		
			if [ $? = 0 ]; then
				echo on >  $SYSFS_OHCI_OMAP/usb2/power/level
			fi
		fi
		lsmod | grep  ohci_hcd
		sleep 10
		cat /proc/bus/usb/devices
		
	fi
sleep 10			
		

elif [ "$LOCAL_COMMAND" = "remove" ]; then
		
		if [ "$LOCAL_DRIVER" = "mentor" ]; then
    		sync
    		rmmod g_zero.ko
		elif [ "$LOCAL_DRIVER" = "ehci" ]; then
			sync
    		rmmod ehci_hcd.ko
    	elif [ "$LOCAL_DRIVER" = "ohci" ]; then
			sync
    		rmmod ohci_hcd.ko
    	fi
		else
   		 exec

fi

# End of file
