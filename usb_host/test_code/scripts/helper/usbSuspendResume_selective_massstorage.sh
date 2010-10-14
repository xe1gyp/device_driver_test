#!/bin/sh
# =============================================================================
# Variables
# =============================================================================

USB=`ls /sys/bus/usb/drivers/usb-storage/ | grep :`
echo $USB
echo `expr substr $USB 1 1`
echo `expr substr $USB 1 3`
USBPORT1=`expr substr $USB 1 1`
echo $USBPORT1
USBPORT2=`expr substr $USB 1 3`
echo $USBPORT2
	
ls /sys/bus/usb/devices/usb$USBPORT1/
ls /sys/bus/usb/devices/usb$USBPORT1/$USBPORT2/power/
chmod 777 /sys/bus/usb/devices/usb$USBPORT1/$USBPORT2/power/control
echo $USB > /sys/bus/usb/drivers/usb-storage/unbind && sleep 10
cat /proc/bus/usb/devices
dmesg -c
echo "Suspending device"
echo auto > /sys/bus/usb/devices/usb$USBPORT1/$USBPORT2/power/control
cat /sys/bus/usb/devices/usb$USBPORT1/$USBPORT2/power/control
dmesg | grep auto-suspend
dmesg -c
echo "Resuming device"
echo on > /sys/bus/usb/devices/usb$USBPORT1/$USBPORT2/power/control
dmesg | grep auto-resume





	
