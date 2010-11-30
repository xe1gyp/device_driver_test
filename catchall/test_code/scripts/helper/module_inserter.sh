#!/bin/sh
################################################################################
# Scenario:    L_DD_SWDEV_0003_0001 and L_DD_SW004_0001
# Author  :    Ricardo Perez Olivares
# Date    :    November 19, 2010
# Description: Checking for Printing Errors and Kernel Version
################################################################################

#Check Kernel version:

kernel_version=`uname -r`
echo "Kernel Version:" $kernel_version
echo -e "\n"

#Check if module debug_module_test.c is available:

ls $MODDIR | grep .ko

if [ $? -eq 0 ]
	then
		echo -e "PASS Module de_bug_module_test.ko found\n"
	else
		echo -e "FAIL, module not available\n"
fi 

insmod $MODDIR/debug_module_test.ko

if [ $? -eq 0 ]
	then
		echo -e "PASS Module inserted properly, please check 7 KERN messages\n"
	else
		echo -e "FAIL, recompile your module\n"
fi 

rmmod debug_module_test.ko

if [ $? -eq 0 ]
	then
		echo -e "PASS Module unloaded successfully\n"
	else
		echo -e "FAIL Module can't be unloaded, please check\n"
fi 

#End of file
