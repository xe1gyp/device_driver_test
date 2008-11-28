#!/bin/bash
#
# (C) Copyright Texas Instruments, 2003. All Rights Reserved.
#
# Name of the file: fb_blank_stress.sh
#
# Location of the file: PET/src/test/os/device_drivers/framebuffer
#
# Brief description of the contents of the file:
#
# 	Blank and unblank the screen repeatedly, test LCD Sync timings
#
#
# Author: Prathibha Tammana
#
# Created on: 9/30/03
#
# Change Log:
# 9/30/03 - Prathibha - Initial Revision
################################################################################



while  true 
do  
	./fbtest -u
	./fbtest -b

done

