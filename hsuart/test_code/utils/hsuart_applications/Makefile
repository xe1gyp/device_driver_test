CC:=$(CROSS_COMPILE)gcc
CFLAGS= -g -c -Wall

all: common.c uart_test.c ts_uart.c
	$(CC)  $(CFLAGS) common.c -o common.o
	$(CC)  $(CFLAGS) uart_test.c -o uart_test.o
	$(CC)  uart_test.o common.o -o uart_test
	$(CC)  ts_uart.o common.o -o ts_uart

install:
	cp uart_test ../../binaries

clean:
	rm -rf *.o uart_test ts_uart
