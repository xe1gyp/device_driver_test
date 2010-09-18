#!/bin/sh

# TODO
# 1. Code can be reduced but need more understanding on OMAP3 implementation
# 2. Add check if value was accepted

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_DOMAIN=$2
LOCAL_ERROR=0

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi

handlerDebugFileSystem.sh "mount"

if [ ! -f $SR_CORE_AUTOCOMP ]; then
	echo "FATAL: $SR_CORE_AUTOCOMP cannot be found!"
	handlerError.sh "log" "1" "halt" "handlerSmartReflex.sh"
	LOCAL_ERROR=1
fi

if [ ! -f $SR_IVA_AUTOCOMP ]; then
	echo "FATAL: $SR_IVA_AUTOCOMP cannot be found!"
	handlerError.sh "log" "1" "halt" "handlerSmartReflex.sh"
	LOCAL_ERROR=1
fi

if [ ! -f $SR_MPU_AUTOCOMP ]; then
	echo "FATAL: $SR_MPU_AUTOCOMP cannot be found!"
	handlerError.sh "log" "1" "halt" "handlerSmartReflex.sh"
	LOCAL_ERROR=1
fi

if [ $LOCAL_ERROR -eq 1 ]; then
	handlerDebugFileSystem.sh "mount"
	exit $LOCAL_ERROR
fi

if [ "$LOCAL_OPERATION" = "enable" ]; then

  if [ "$LOCAL_DOMAIN" = "core" ]; then
    echo "echo $PM_ENABLE > $SR_CORE_AUTOCOMP"
    echo $PM_ENABLE > $SR_CORE_AUTOCOMP
  elif [ "$LOCAL_DOMAIN" = "iva" ]; then
    echo "echo $PM_ENABLE > $SR_IVA_AUTOCOMP"
    echo $PM_ENABLE > $SR_IVA_AUTOCOMP
  elif [ "$LOCAL_DOMAIN" = "mpu" ]; then
    echo "echo $PM_ENABLE > $SR_MPU_AUTOCOMP"
    echo $PM_ENABLE > $SR_MPU_AUTOCOMP
  fi


elif [ "$LOCAL_OPERATION" = "disable" ]; then

  if [ "$LOCAL_DOMAIN" = "core" ]; then
    echo "echo $PM_ENABLE > $SR_CORE_AUTOCOMP"
    echo $PM_DISABLE > $SR_CORE_AUTOCOMP
  elif [ "$LOCAL_DOMAIN" = "iva" ]; then
    echo "echo $PM_ENABLE > $SR_IVA_AUTOCOMP"
    echo $PM_DISABLE > $SR_IVA_AUTOCOMP
  elif [ "$LOCAL_DOMAIN" = "mpu" ]; then
    echo "echo $PM_ENABLE > $SR_MPU_AUTOCOMP"
    echo $PM_DISABLE > $SR_MPU_AUTOCOMP
  fi

else
  echo "Fatal: Operation in smartReflexAutoComp is not supported!"
  LOCAL_ERROR=1
fi

handlerDebugFileSystem.sh "umount"

exit $LOCAL_ERROR

# End of file