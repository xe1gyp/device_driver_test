CC:=$(CROSS_COMPILE)gcc
UFLAGS:= $(CFLAGS)
CFLAGS:= -g -c -Wall

all: spidev_test.c spidev_fdx.c
	$(CC)  $(CFLAGS) spidev_test.c -o spidev_test.o
	$(CC)  $(CFLAGS) spidev_fdx.c -o spidev_fdx.o

install:
	cp spidev_test.o spidev_fdx.o  ../../bin/

clean:
	rm -rf *.o
