#!/bin/sh
# ##########################################################################
# (C) Copyright Texas Instruments. All Rights Reserved.
#
# Name of the file: buildTestSuites.sh
#
# Script to Build OMAP Test Suites for OMAP4
#
# 4 variables need to be exported
# export CROSS_COMPILE=arm-non-linux-gneuabi-
# export KDIR=/path/to/kernel/directory
# export HOST=arm-none-linux-gnueabi
# export TESTSUITES="dma mcbsp" || export TESTSUITES="all"
# export ARCH=arm
#
# ###########################################################################

if [ "$#" -lt 1 ] 
then
	echo -e "\nbuildTestSuites <output.directory>\n"
	exit 1
fi

OUTPUT_DIRECTORY=$1
ROOT=`pwd`

# Verify needed variables are exported

if [ "$TESTSUITES" == "" ]
then
        echo "Please export your TESTSUITES variable"
	echo "TESTSUITES value can be any name of device driver under this directory or \"all\" to compile everything"
	echo "Example: export TESTSUITES=\"dma gpio, i2c\""
        exit 1

elif [ "$TESTSUITES" == "all" ]
then
        TESTSUITES="audio-alsa dma framebuffer gpio hsuart \
i2c mcbsp mcspi mmc nand norflash performance realtimeclock \
timer-32k video watchdog"

fi

echo -e "\nThe following test suites will be compiled: $TESTSUITES\n" && sleep 1

if [ -z $CROSS_COMPILE ]
then
	echo -e "\nPlease export your CROSS_COMPILE variable\n"
	exit 1
fi
 
if [ -z $KDIR ]
then
        echo -e "\nPlease export your KDIR variable\n"
	exit 1
fi

if [ -z $HOST ]
then
        echo -e "\nPlease export your HOST variable\n"
	exit 1
fi

if [ -z $ARCH ]
then
	echo -e "\nPlease export your ARCH variable\n"
	exit 1
fi
 

export CROSS_COMPILE
export KDIR
export HOST

OUTPUT_DIRECTORY=$OUTPUT_DIRECTORY/testsuites

echo -e "\nThe kernel source code for the compilation is located at $KDIR"
echo -e "Testsuite(s) will be copied to $OUTPUT_DIRECTORY\n"

if [ -d $OUTPUT_DIRECTORY ]
then
	echo -e "WARNING: This script will delete the testsuites directory of your output folder to create"
	echo -e "a clean copy from Clear Case, do you want to continue (Y||y/N||n)? "
	read answer
	if [[ ($answer = "Y") || ($answer = "y") ]]
	then
		echo -e "Erasing output directory"
		rm -rf $OUTPUT_DIRECTORY/*
	else
		exit 1
	fi
else
	mkdir $OUTPUT_DIRECTORY
	if [ -d $OUTPUT_DIRECTORY ]
	then
		echo -e "\nTestsuites directory succesfully created\n"
	else
		echo -e "\nError! Testsuites directory coudl not be created\n"
		exit 1
	fi
fi

# Compile all drivers
for DRIVER in $TESTSUITES
do 
	cd $ROOT/$DRIVER/test_code/
	make
	mkdir $OUTPUT_DIRECTORY/$DRIVER
	cp -r $ROOT/$DRIVER/test_code/* $OUTPUT_DIRECTORY/$DRIVER/
	if [ -d "$OUTPUT_DIRECTORY/$DRIVER/utils" ]; then
		#rm -rf $OUTPUT_DIRECTORY/$DRIVER/utils
		continue
	fi
	make clean
done

# Compile utils where common applications for all drivers are located
cd $ROOT/utils
make
cp -r $ROOT/utils $OUTPUT_DIRECTORY/
make clean

# End of buildTestSuites.sh script
