#!/bin/bash
###########################################################################
# (C) Copyright Texas Instruments, 2009. All Rights Reserved.
#
# buildTestSuites.sh - Script to Build OMAP Test Suites
#
# Description:
# 	This script will compile available test suites
#
# NOTE: 3 variables need to be exported:
#	CROSS_COMPILE=<tool.chain.prefix>
#	KDIR=<path.to.kernel.source>
#	HOST=<host.prefix>
#
# Author:
#	Abraham Arce <x0066660@ti.com>
#
# Created on: Sun Mar  9 14:40:00 CDT 2008
#
# Change Log:
#	03/09/2008 (Abraham Arce <x0066660@ti.com>)
#		- Initial Version
#	09/24/2008 (Omar Jimenez)
#		- Adding option to compile and copy only the desired driver
#		  test suite.
#		- Adding warning message of testsuites directory erasing
#	01/07/2009 (Diego Zavala Trujillo)
#		- Correct the way to copy the test_code to the target.
#	01/08/2009 (Diego Zavala Trujillo)
#		- Add usb device host and otg for compilation.
#	08/10/2009 (Sergio Aguirre <saaguirre@ti.com>)
#		- Small cleanup and optimization
#
############################################################################

if [ "$#" -lt 1 ] 
then
	echo; echo "buildTestSuites <output.directory>"; echo
	exit 1
fi

OUTPUT_DIRECTORY=$1
TESTSUITES_ROOT=`pwd`
DRIVERS_OBSOLETE="led sound_services"

if [ -z $DRIVERS ]
then
	# Compiling all drivers test suites
	# Missing Test Suites > sdio, timer-32k, hdq
	DRIVERS="audio-alsa camera dma ethernet gpio framebuffer i2c keypad \
mcbsp mmc nand norflash power_management realtimeclock \
touchscreen video watchdog camera usb_device usb_host \
usb_otg usb_ehci"
fi	

# Verify the current FS to add new CFLAGS in the compilation

if [ "$FSTYPE" == "android" ]
then
	export CFLAGS+=" -static"
fi

# Verify needed variables are exported

if [ "$TESTSUITES" == "" ]
then
        echo "Please export your TESTSUITES variable"
	echo "TESTSUITES value can be any name of device driver under this directory or all to compile everything"
	echo "Example: export TESTSUITES=\"dma gpio, i2c\""
        exit 1
fi

if [ "$TESTSUITES" == "all" ]
then
	TESTSUITES="audio-alsa camera dma ethernet gpio framebuffer i2c \
keypad mcbsp mmc nand norflash power_management \
realtimeclock touchscreen video watchdog usb_device \
usb_host usb_otg usb_ehci"
fi

echo; echo "The following test suites will be compiled: $TESTSUITES"; echo; sleep 3

if [ -z $CROSS_COMPILE ]
then
	echo "Please export your CROSS_COMPILE variable"
	exit 1
fi
 
if [ -z $KDIR ]
then
        echo "Please export your KDIR variable"
	exit 1
fi

if [ -z $HOST ]
then
        echo "Please export your HOST variable"
	exit 1
fi

export CROSS_COMPILE
export KDIR
export HOST
export TESTSUITES_ROOT

OUTPUT_DIRECTORY=$OUTPUT_DIRECTORY/testsuites

echo
echo "Your source code for the compilation is located at $KDIR"
echo
echo "The testsuite(s) will be copy to $OUTPUT_DIRECTORY"
echo

if [ -d $OUTPUT_DIRECTORY ]
then
	echo "WARNING: This script will delete the testsuites directory of your output folder to create"
	echo "a clean copy from Clear Case, do you want to continue (Y/N)?"
	read answer
	if [[ ($answer = "Y") || ($answer = "y") ]]
	then
		echo "Erasing output directory"
		rm -rf $OUTPUT_DIRECTORY/*
	else
		exit 1
	fi
else
	mkdir -p $OUTPUT_DIRECTORY
	if [ -d $OUTPUT_DIRECTORY ]
	then
		echo "testsuites directory succesfully created"
	else
		echo "CAN NOT CREATE TESTSUITES DIRECTORY"
		exit 1
	fi
fi

# Compile all drivers
for DRIVER in $TESTSUITES
do 
	make -C $TESTSUITES_ROOT/$DRIVER/test_code/
	mkdir $OUTPUT_DIRECTORY/$DRIVER
	cp -r $TESTSUITES_ROOT/$DRIVER/test_code/* $OUTPUT_DIRECTORY/$DRIVER/
	make -C $TESTSUITES_ROOT/$DRIVER/test_code/ clean
done

# Compile utils
make -C $TESTSUITES_ROOT/utils
cp -r $TESTSUITES_ROOT/utils $OUTPUT_DIRECTORY/
make -C $TESTSUITES_ROOT/utils/ clean
# Change permissions
chmod -R 755 $OUTPUT_DIRECTORY

#s End of buildTestSuites.sh script
