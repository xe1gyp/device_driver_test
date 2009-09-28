#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_COMMAND=$1
LOCAL_INSTANCE=$2

echo > $GENERAL_LOG_FILE.$LOCAL_INSTANCE

# =============================================================================
# Functions
# =============================================================================

logIt() {

  echo -e "$1"
  echo -e "$1" >> $GENERAL_LOG_FILE.$LOCAL_INSTANCE

}

logIt "\n"
logIt "# ============================================================================="
logIt "  Info: $LOCAL_COMMAND"
logIt "# ============================================================================="
logIt "\n"

$UTILS_DIR_HANDLERS/handlerAppTop2.sh "start"

$TIME_APPLICATION/time -p -o $TIME_EXEC_LOG $LOCAL_COMMAND

$UTILS_DIR_HANDLERS/handlerAppTop2.sh "stop"
$UTILS_DIR_HANDLERS/handlerAppTop2.sh "parse" "User"
$UTILS_DIR_HANDLERS/handlerAppTop2.sh "parse" "System"

cat $CPU_LOG_FILE >> $GENERAL_LOG_FILE.$LOCAL_INSTANCE
cat $GENERAL_LOG_FILE.$LOCAL_INSTANCE >> $GENERAL_LOG_FILE
cat $TIME_EXEC_LOG >> $GENERAL_LOG_FILE

# End of file
