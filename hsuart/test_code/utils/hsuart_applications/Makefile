CC:=$(CROSS_COMPILE)gcc
CFLAGS= -g -c -Wall

all: common.c uart_test.c
	$(CC)  $(CFLAGS) common.c -o common.o
	$(CC)  $(CFLAGS) uart_test.c -o uart_test.o
	$(CC)  uart_test.o common.o -o uart_test

install:
	cp uart_test ../../binaries

clean:
	rm -rf *.o uart_test
