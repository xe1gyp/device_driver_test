#!/bin/sh

set -x
SEND_RECEIVE=$1
SAMPLE_FILE=$2
BAUDRATE=$3
FLOW_CONTROL=$4
NEGATIVE_TC=$5

if [ "$SEND_RECEIVE" = "s" ]; then
  echo "You have 10 seconds to execute ts_uart in the second Board"

  if [ "$NEGATIVE_TC" = "$NORMAL" ]; then
    echo "-> Suggestion to receive:"
    echo " ./ts_uart r file_sent $BAUDRATE $FLOW_CONTROL"

  elif [ "$NEGATIVE_TC" = "$DISCONNECT" ]; then
    echo "Please disconnect the serial cable before starting the data transfer."
    echo "Timeout on the receiver shall expire and the file should not be transfered."
    echo "-> Suggestion to receive:"
    echo " ./ts_uart r file_sent $BAUDRATE $FLOW_CONTROL"

  elif [ "$NEGATIVE_TC" = "$WRONG_BAUDRATE" ]; then

    if [ "$BAUDRATE" = "115200" ]; then
      WRONG_BAUDRATE=`expr 3500000`
    else
      WRONG_BAUDRATE=`expr 115200`
    fi

    echo "Data transfer should not be completed successfully configuring different BAUD RATES on the receiver and transmitter"
    echo "-> Suggestion to receive:"
    echo " ./ts_uart r file_sent $WRONG_BAUDRATE $FLOW_CONTROL"

  elif [ "$NEGATIVE_TC" = "$WRONG_FLOW_CONTROL" ]; then

    if [ "$FLOW_CONTROL" = "0" ]; then
      WRONG_FLOW_CONTROL=`expr 1`
    else
      WRONG_FLOW_CONTROL=`expr 0`
    fi

    echo "Data transfer should not be completed successfully configuring different FLOW CONTROL types on the receiver and transmitter"
    echo "-> Suggestion to receive:"
    echo " ./ts_uart r file_sent $BAUDRATE $WRONG_FLOW_CONTROL"
  fi

  sleep 10
  $TS_UART $SEND_RECEIVE $SAMPLE_FILE $BAUDRATE $FLOW_CONTROL

elif [ "$SEND_RECEIVE" = "r" ]; then
    echo "To receive, you should run the command manually in the other board"
    exit 1
else
    echo "Command in transmittingData is not supported"
    exit 1
fi
