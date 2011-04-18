#!/bin/sh

##############################################################################
# Author      : Govindraj.R
# Date        : Apr 30 2010
# Description : Used to run rx process in background and then tx.
#		other details needed will be passed from Scenario ID.
##############################################################################

#ts_uart r $TTY_INTERFACE $BAUDRATE $FLOWCONTROL &
#ts_uart s $TTY_INTERFACE $BAUDRATE $FLOWCONTROL
$HSUART_DIR_BINARIES/ts_uart r $1 $2 $3 &
$HSUART_DIR_BINARIES/ts_uart s $1 $2 $3
exit 0;
