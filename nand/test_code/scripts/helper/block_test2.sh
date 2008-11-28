#!/bin/sh

OFFSET=$1
SIZE=$2
ERASE_BLOCKS=$3

test -e $TMPBASE/file1 && rm $TMPBASE/file1
test -e $TMPBASE/file2 && rm $TMPBASE/file2
$TESTBIN/nanddump $MTD_CHAR_DEV1 -f $TMPBASE/file1 -s $OFFSET -l $SIZE &&
$TESTBIN/flash_erase $MTD_CHAR_DEV1 $OFFSET $ERASE_BLOCKS &&
$TESTBIN/nandwrite -o -s $OFFSET $MTD_CHAR_DEV1 $TMPBASE/file1 &&
$TESTBIN/nanddump $MTD_CHAR_DEV1 -f $TMPBASE/file2 -s $OFFSET -l $SIZE &&
cmp $TMPBASE/file1 $TMPBASE/file2 && exit 0 || exit 1
