#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_COMMAND=$1
LOCAL_DRIVER=$2

# =============================================================================
# Functions
# =============================================================================


# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_COMMAND" = "create" ]; then
	
	mount | grep usbfs || mount -t usbfs none /proc/bus/usb/
	
	if [ "$LOCAL_DRIVER" = "mentor" ]; then
	
		echo "Inserting the module"
		insmod $UTILSMODULES/g_zero.ko
		lsmod | grep  g_zero
		echo "otg" > $USBHOST_ENUMERATION
		
	elif [ "$LOCAL_DRIVER" = "usbtest" ]; then
	
		echo "Inserting the module"
		insmod $UTILSMODULES/usbtest.ko
		lsmod | grep  usbtest
		
		
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
					else 
						ls $SYSFS_EHCI_OMAP/usb3
						if [ $? = 0 ]; then
						echo on >  $SYSFS_EHCI_OMAP/usb3/power/level
						fi
					fi
		fi			
	
	lsmod | grep  ehci_hcd
		
	
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
					else
						ls $SYSFS_OHCI_OMAP/usb3
						if [ $? = 0 ]; then
						echo on >  $SYSFS_OHCI_OMAP/usb3/power/level
						fi
					fi
		fi
		
		lsmod | grep  ohci_hcd
				
	fi
	
	sleep 10


elif [ "$LOCAL_COMMAND" = "remove" ]; then
		
		if [ "$LOCAL_DRIVER" = "mentor" ]; then
    		sync
    		rmmod g_zero.ko
    	
    	elif [ "$LOCAL_DRIVER" = "usbtest" ]; then
			sync
    		rmmod usbtest.ko
    		
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
