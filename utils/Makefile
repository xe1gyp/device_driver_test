
CC := $(CROSS_COMPILE)gcc

export UTIL_DIR = $(PWD)
export UTIL_DESTDIR = ($PWD)/bin

BINARIES:=pan/pan akey/akey akey/waitkey akey/choice

all: pan_app akey_app install

install:
	mkdir -p bin
	cp $(BINARIES) bin/

pan_app:
	make -C pan CC=$(CC)
	
akey_app:
	make -C akey CC=$(CC)

clean:
	make -C pan clean
	make -C akey clean
	rm -rf bin
