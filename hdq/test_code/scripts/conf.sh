#!/bin/sh

# TestSuite General Variables
export HDQ_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export HDQ_ROOT=`pwd`

export HDQ_DIR_HELPER=${HDQ_ROOT}/helper
export HDQ_DIR_TMP=${HDQ_ROOT}/tmp
export HDQ_DIR_TEST=${HDQ_ROOT}/test
export HDQ_DIR_SCENARIOS="${HDQ_ROOT}/scenarios"

export HDQ_FILE_OUTPUT=${HDQ_ROOT}/output.$HDQ_POSTFIX
export HDQ_FILE_LOG=${HDQ_ROOT}/log.$HDQ_POSTFIX
export HDQ_FILE_TMP=${HDQ_DIR_TMP}/tmp.$HDQ_POSTFIX
export HDQ_FILE_CMD=cmd.$HDQ_POSTFIX

export HDQ_DURATION=""
export HDQ_PRETTY_PRT=""
export HDQ_VERBOSE=""
export HDQ_SCENARIO_NAMES=""
export HDQ_STRESS=""

export PATH="${HDQ_ROOT}:${HDQ_DIR_HELPER}:${PATH}"

# End of file