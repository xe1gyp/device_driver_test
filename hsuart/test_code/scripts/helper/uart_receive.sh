#!/bin/sh
#Invoke uart receive
rm -f uart_rx_file
$MODDIR/ts_uart r $UART_TEST_PORT1 uart_rx_file 115200 0
