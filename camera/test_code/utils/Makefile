export CC:=$(CROSS_COMPILE)gcc

PWD=$(shell pwd)

all: tc

clean:
	make -C testcode/ clean

install:
	make -C testcode/ install

tc: 
	make -C testcode/
