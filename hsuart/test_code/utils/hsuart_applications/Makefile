export CC:=$(CROSS_COMPILE)gcc
export CFLAGS= -g -c -Wall
OBJS:=ts_uart

ts_uart : common.c ts_uart.c
	$(CC)  $(CFLAGS) common.c -o common.o
	$(CC)  $(CFLAGS) ts_uart.c -o ts_uart.o
	$(CC)  ts_uart.o common.o -o ts_uart
install:
	cp $(OBJS) ../../bin/
clean :
	rm -rf *.o ts_uart
