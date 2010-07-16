#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_ARGUMENTS=$1

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

ping $LOCAL_ARGUMENTS

# End of file
