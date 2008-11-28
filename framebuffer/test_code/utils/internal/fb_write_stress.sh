#!/usr/bin/env bash
#
# (C) Copyright Texas Instruments, 2003. All Rights Reserved.
#
# Name of the file: fb_write_stress.sh
#
# Location of the file: PET/src/test/os/device_drivers/framebuffer
#
# Brief description of the contents of the file:
#
#	Write screenful of data, repeatedly
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
	./fbtest  -w -l 0x4b000 -f 0x0fab
	./fbtest  -w -l 0x4b000 -f 0x0bad
	./fbtest  -w -l 0x4b000 -f 0x0add
	./fbtest  -w -l 0x4b000 -f 0x0fff
	./fbtest  -w -l 0x4b000 -f 0x0000
	./fbtest  -w -l 0x4b000 -f 0x0ff0
	./fbtest  -w -l 0x4b000 -f 0x00f2

done

