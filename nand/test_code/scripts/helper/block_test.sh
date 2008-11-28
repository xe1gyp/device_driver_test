#!/bin/sh

OFFSET1=$1
OFFSET2=$2
OFFSET3=$3
SIZE=$4
ERASE_BLOCKS=$5

test -e $TMPBASE/file1 && rm $TMPBASE/file1
test -e $TMPBASE/file2 && rm $TMPBASE/file2
$TESTBIN/nanddump $MTD_CHAR_DEV1 -f $TMPBASE/file1 -s $OFFSET1 -l $SIZE &&
$TESTBIN/flash_erase $MTD_CHAR_DEV1 $OFFSET3 $ERASE_BLOCKS &&
$TESTBIN/nandwrite -o -s $OFFSET2 $MTD_CHAR_DEV1 $TMPBASE/file1 &&
$TESTBIN/nanddump $MTD_CHAR_DEV1 -f $TMPBASE/file2 -s $OFFSET2 -l $SIZE &&
cmp $TMPBASE/file1 $TMPBASE/file2 && exit 0 || exit 1
