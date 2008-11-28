export CC:=$(CROSS_COMPILE)gcc

PWD=$(shell pwd)

all: fbdev inter

clean:
	make -C FBdev/utilities/fbtest/ clean
	make -C internal/ clean

install:
	make -C internal/ install
	make -C FBdev/utilities/fbtest/ install
fbdev: 
	make -C FBdev/utilities/fbtest/

inter:
	make -C internal/ 

