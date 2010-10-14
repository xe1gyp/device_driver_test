#!/bin/sh

LOCAL_DRIVER=$1

DVFS-test-script.sh &

if [ "$LOCAL_DRIVER" = "musb" ]; then
	echo "Load/Unload Test for MUSB"
	for i in `seq 1 20`; do
		echo " Load MUSB module"
		insmod $UTILSMODULES/musb-hcd.ko && sleep 1
		echo " Unoad MUSB module"
		rmmod musb-hcd
	done
fi

if [ "$LOCAL_DRIVER" = "ehci" ]; then
	echo "Load/Unload Test for EHCI"
	for i in `seq 1 20`; do
		echo " Load EHCI module"
		insmod $UTILSMODULES/ehci-hcd.ko && sleep 1
		echo " Unoad EHCI module"
		rmmod ehci-hcd
	done
fi


if [ "$LOCAL_DRIVER" = "ohci" ]; then
	echo "Load/Unload Test for OHCI"
	for i in `seq 1 20`; do
		echo " Load EHCI module"
		insmod $UTILSMODULES/ohci-hcd.ko && sleep 1
		echo " Unoad EHCI module"
		rmmod ohci-hcd
	done
fi

#pkill -x DVFS-test-script.sh





	
