#! /bin/bash 

#-----------------------------------------------------------------------
# Initialize local variables
#-----------------------------------------------------------------------
TCbin=${TCbin:=`pwd`}
TCtmp=${TCtmp:=/tmp/$TC$$}
export PATH=$PATH:$TCbin:../../../bin
export TCID=$TC
export TST_TOTAL=1
export TST_COUNT=1
TARGET_DIR=$1

if [ ! -d "$TARGET_DIR" ]; then
	tst_resm TBROK "no Target dir: $TARGET_DIR"
	exit 1
fi
cd $TARGET_DIR
tst_resm TINFO "## Start Test2"
date 
date +%s
if [ -d ./00 ] ; then
/bin/rm -fr ./00 >& /dev/null
fi
STARTT=`date +%s`
echo $STARTT

echo ""
echo "## Create files "
time $TCbin/cr 

echo ""
echo "## tar all "
MAXFILE=`tar cBf - 00 | tar tvBf - 2>&1 | tail -1 | awk '{print $6;}'| awk -F'/' '{print $4;}'`
HALFFILE=`echo "obase=F;ibase=F;$MAXFILE/2" | bc`

echo ""
echo "## Remove all files and directories"
/bin/rm -fr ./00 >& /dev/null

echo ""
echo "## Create half files"
echo create half files
time $TCbin/cr  $HALFFILE

echo ""
echo "## Change owner"
time chown -R $USER  ./00

echo ""
echo "## random access"
time $TCbin/ra  $HALFFILE


echo ""
echo "## Change mode "
time chmod -R go+rw  ./00

echo ""
echo "## Random delete and create"
time $TCbin/radc  $HALFFILE

echo ""
echo "## Change mode again"
time chmod -R go-rw  ./00

echo ""
echo "## Remove all files and directories"
time /bin/rm -fr ./00

echo ""
echo "## Finish test"
date
ENDT=`date +%s`
echo $ENDT
echo -n 'TOTAL(seconds): ' 
expr $ENDT - $STARTT


