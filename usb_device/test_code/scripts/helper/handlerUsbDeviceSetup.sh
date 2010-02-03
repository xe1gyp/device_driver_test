#!/bin/sh
set -x
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

	export USB_TEMP_FILE=${TMPBASE}/usb.ext2.temporal.file
	
	if [ "$LOCAL_DRIVER" = "file_storage" ]; then
    
		echo "Inserting the module"	
		
			mkdir -p /mnt/ramdrive
			echo "Command: dd if=/dev/urandom of=/mnt/ramdrive/usb.ext2.temporal.file bs=1M count=50"
			dd if=/dev/urandom of=/mnt/ramdrive/usb.ext2.temporal.file bs=1M count=5; sync
			ls -lah /mnt/ramdrive/usb.ext2.temporal.file
			echo "Command: insmod g_file_storage file=usb.ext2.temporal.file stall=n removable=y buflen=1048576"
			echo -e "\n" |insmod $USBDEVICE_DIR_MODULES/g_file_storage.ko file=/mnt/ramdrive/usb.ext2.temporal.file stall=n removable=y
			echo -e "y\n"| mkdosfs /mnt/ramdrive/usb.ext2.temporal.file
			lsmod | grep g_file_storage

	elif [ "$LOCAL_DRIVER" = "zero" ]; then
	
		echo -e "\n" |insmod $USBDEVICE_DIR_MODULES/g_zero.ko
		lsmod | grep g_zero
		lsmod
	
	elif [ "$LOCAL_DRIVER" = "serial" ]; then
	
		echo -e "\n" |insmod $USBDEVICE_DIR_MODULES/g_serial.ko use_acm=1
		lsmod | grep g_serial
		
	elif [ "$LOCAL_DRIVER" = "ether" ]; then
		
		#	Where host_addr will be HOST MAC
		#	and dev_addr will be MA
		
		echo -e "\n" |insmod $USBDEVICE_DIR_MODULES/g_ether.ko host_addr=D6:F7:63:A9:F5:3B dev_addr=00:11:22:33:44:ee
		lsmod | grep g_ether
	fi
	

elif [ "$LOCAL_COMMAND" = "remove" ]; then
	
	if [ "$LOCAL_DRIVER" = "ether" ]; then   
				sync
				rmmod g_ether
	 			
	 			
	elif [ "$LOCAL_DRIVER" = "serial" ]; then   
				sync
				rmmod g_serial
	 				
	elif [ "$LOCAL_DRIVER" = "zero" ]; then   
				sync
				rmmod g_zero
	 			
	elif [ "$LOCAL_DRIVER" = "file_storage" ]; then
		sync
		rmmod g_file_storage
		
	fi
		lsmod
	else
	exec

fi

# End of file
