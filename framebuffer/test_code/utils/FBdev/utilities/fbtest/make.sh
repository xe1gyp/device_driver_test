#!/bin/bash
make clean
cp ../images-bak/penguin.c images/penguin.c
export CROSS_COMPILE=arm-none-linux-gnueabi-
make
