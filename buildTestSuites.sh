#!/bin/sh
###########################################################################
# (C) Copyright Texas Instruments, 2008. All Rights Reserved.
#
# Name of the file: buildTestSuites.sh
#
# Location of the file: /vobs/wtbu/OMAPSW_L/linux/test/device_driver_test
#
# Brief description of the contents of the file:
# Script to Build OMAP Test Suites
#
# Detailed description of the contents of the file:
# This script will compile available test suites 
#
# 3 variables need to be exported:
# exported CROSS_COMPILE=<tool.chain.prefix>
# KDIR=<path.to.kernel>
# HOST=<host.prefix>
# 
# Author: Abraham Arce 
#
# Created on: Sun Mar  9 14:40:00 CDT 2008
#
# Change Log:
#		Abraham Arce - 03/09/2008 - Initial Version
#		Omar Jimenez - 09/24/2008 - Adding option to compile and copy
#					    only the desired driver test suite
#					    Adding warning message of 
#			 		    testsuites directory erasing
#		Diego Zavala Trujillo - 01/07/2009 - Correct the way to copy
#						the test_code to the target.
#
############################################################################

if [ "$#" -lt 1 ] 
then
	echo; echo "buildTestSuites <output.directory>"; echo
	exit 1
fi

OUTPUT_DIRECTORY=$1
ROOT=`pwd`
DRIVERS_OBSOLETE="led sound_services"

if [ -z $DRIVERS ]
then
	# Compiling all drivers test suites
	# Missing Test Suites > sdio, timer-32k, hdq
	DRIVERS="audio-alsa dma ethernet gpio framebuffer i2c keypad mcbsp mmc nand norflash power_management realtimeclock touchscreen video watchdog camera"
fi	
# TESTSUITES="audio-alsa dma ethernet gpio framebuffer i2c keypad mcbsp mmc nand norflash power_management realtimeclock touchscreen video watchdog"

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
	TESTSUITES="audio-alsa dma ethernet gpio framebuffer i2c keypad mcbsp mmc nand norflash power_management realtimeclock touchscreen video watchdog"
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
	mkdir $OUTPUT_DIRECTORY
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
	cd $ROOT/$DRIVER/test_code/
	make clean
	make
	cd $OUTPUT_DIRECTORY
	mkdir $DRIVER
	cp -r $ROOT/$DRIVER/test_code/* $OUTPUT_DIRECTORY/$DRIVER/
done

# Compile utils
cd $ROOT/utils
#make clean
make
cp -r $ROOT/utils $OUTPUT_DIRECTORY/

# Change permissions
chmod -R 755 $OUTPUT_DIRECTORY

#s End of buildTestSuites.sh script
