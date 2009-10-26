CC:=$(CROSS_COMPILE)gcc
CFLAGS= -g -c -Wall

all: common.c ts_uart.c
	$(CC)  $(CFLAGS) common.c -o common.o
	$(CC)  $(CFLAGS) ts_uart.c -o ts_uart.o
	$(CC)  ts_uart.o common.o -o ts_uart

install:
	cp ts_uart ../../binaries

clean:
	rm -rf *.o ts_uart
