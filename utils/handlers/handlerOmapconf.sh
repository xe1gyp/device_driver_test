#!/bin/bash

#
# OMAPCONF Tool handler
# @author: Leed Aguilar
#

# =============================================================================
# Variables
# =============================================================================


# TODO List
# 1. Complete omapconf abstraction


# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_DOMAIN=$2
LOCAL_FILE1=$3
LOCAL_FILE2=$4
LOCAL_COMPARE=$5
LOCAL_TMP1="value1.log"
LOCAL_TMP2="value2.log'"
LOCAL_OMAPCONF_EXECUTE=()
LOCAL_OMAPCONF_DOMAIN=()
LOCAL_OUTPUT_COLUMN=0
LOCAL_FLAG_DIFFER=0
LOCAL_FLAG_DOMAIN_CHANGE=0
LOCAL_ERROR=0
LOCAL_OMAPCONF="$UTILS_DIR_BIN/omapconf"

# =============================================================================
# OMAPCONF test domains
# =============================================================================

LOCAL_OMAPCONF_DOMAINS=( VOLTAGE FREQUENCY CLOCKS )

LOCAL_CLOCK_DOMAINS=( WKUP_L4_ICLK2 WKUP_32K_GFCLK GPT1_FCLK WKUP_32K_GFCLK \
		     FUNC_32K_CLK L4_ICLK2 CORE_DPLL_EMU_CLK MPU_DPLL_CLK \
		     AESS_FCLK DMIC_ABE_FCLK ABE_ICLK2 MCASP1_FCLK MCBSP1_FCLK \
		     MCBSP2_FCLK MCBSP3_FCLK PAD_CLKS SLIMBUS_UCLKS \
		     ABE_GPT5_FCLK ABE_GPT6_FCLK ABE_GPT7_FCLK ABE_GPT8_FCLK \
		     ABE_ALWON_32K_CLK IVAHD_ROOT_CLK DSP_ROOT_CLK \
		     SR_CORE_SYS_CLK SR_MPU_SYS_CLK SR_IVA_SYS_CLK CFG_L4_ICLK \
		     C2C_L3X2_ICLK C2C_L3_ICLK C2C_L4_ICLK DMA_L3_ICLK \
		     MPU_M3_CLK L3_ICLK1 L3_ICLK2 L3_INSTR_GICLK EMIF_L3_ICLK \
		     DLL_CLK STD_EFUSE_SYS_CLK ISS_CLK FDIF_FCLK DSS_FCLK \
		     HDMI_PHY_48M_FCLK SGX_FCLK HSI_FCLK MMC1_FCLK MMC2_FCLK \
		     INIT_48MC_FCLK OTG_60M_FCLK INIT_48M_FCLK INIT_60M_FCLK \
		     L4_ICLK GPT2_FCLK GPT3_FCLK GPT4_FCLK GPT9_FCLK GPT10_FCLK \
		     GPT11_FCLK PER_32K_GFCLK PER_48M_FCLK SLIMBUS_CORE_UCLKS \
		     PER_L4_ICLK 12M_FCLK PER_MCBSP4_FCLK )

LOCAL_FREQ_DOMAINS=( MPU TESLA IVAHD ABE L3 L4 SGX FDIF DUCATI DSS HSI )

LOCAL_VOLT_DOMAINS=( VDD_MPU VDD_IVA VDD_CORE )

# =============================================================================
# Functions
# =============================================================================

# Display the script usage
# @ Function  : generalUsage
# @ parameters: None
# @ Return    : Error flag value
generalUsage() {
	echo -e "\n  -------------------------------------------------\n"
	echo -e "  Try - handlerOmapconf.sh {run|compare} <OPTIONS>\n"
	echo -e "  Example with "run" command:"
	echo -e "  ./handlerOmapconf run <function>\n"
	echo -e "  Example with "compare" command:"
	echo -e "  ./handlerOmapconf compare <function> <LOG1> <LOG2>" \
		"\n\t\t    {match|differ}\n"
	echo -e "  Where:\n"
	echo -e "  <function> can be:"
	for index in ${!LOCAL_OMAPCONF_OPERATIOvN[*]}; do
		echo "	- ${LOCAL_OMAPCONF_DOMAINS[$index]}"
	done
	echo -e "  {match|differ} are comparison flags"
	echo -e "\n  -------------------------------------------------\n"
	LOCAL_ERROR=1
}

# This function checks if a parameter
# is declared inside of an array
# @ Function  : checkParamInArray
# @ Parameters: <parameter> <array>
# @ Return    : Error flag value
checkParamInArray() {
	param=$1
	array_name=$2
	# Saving all the arrays values into a temporal variable
	eval "eval 'echo "\${$array_name[@]}"'" > temp
	if [ ! `cat temp | grep -wc $param` -gt 0 ]; then
		echo -e "\nERROR: $param is not declared in $array_name array\n"
		LOCAL_ERROR=1
	fi
	rm temp
}

# This function verify is the file(s) given exist
# @ Function  : verifyFiles
# @ Parameters: <file1> <file2> <...>
# @ Return    : Error flag value
verifyFiles() {
	files=( "$@" )
	for index in ${!files[@]}; do
		if [ ! -f ${files[$index]} ]; then
			showInfo "ERROR: ${files[$index]}  does not exist" 1>&2
			LOCAL_ERROR=1
		fi
	done
}

# Compares similar values inside of different omapconf tables
# The functions takes the following predefined variables:
# - OMAPCONF domain array
# - Two logs containing omapconf tables
# - Column number inside of the table
# @ Function  : compareOmapconfResults
# @ Parameters: None
# @ Return    : Error flag value
compareOmapconfResults() {
	echo -e "\n\n------------------- COMPARING $LOCAL_DOMAIN DOMAINS -------------------\n"
	for index in ${!LOCAL_OMAPCONF_DOMAIN[*]}; do
		grep -m 1 -w "${LOCAL_OMAPCONF_DOMAIN[$index]}" $LOCAL_FILE1 \
				| awk '{printf$'$LOCAL_OUTPUT_COLUMN'}' > $LOCAL_TMP1
		grep -m 1 -w "${LOCAL_OMAPCONF_DOMAIN[$index]}" $LOCAL_FILE2 \
				| awk '{printf$'$LOCAL_OUTPUT_COLUMN'}' > $LOCAL_TMP2
		val1=`cat $LOCAL_TMP1`
		val2=`cat $LOCAL_TMP2`
		if [ $val1 != $val2 ]; then
			LOCAL_FLAG_DOMAIN_CHANGE=1
			if [ "$LOCAL_COMPARE" == "match" ]; then
				echo -e "\t\tFAIL | [${LOCAL_OMAPCONF_DOMAIN[$index]}] domain differ"
				echo -e "\t\t     | Current  value: $val1"
				echo -e "\t\t     | Expected value: $val2\n"
				LOCAL_ERROR=1
			elif [ "$LOCAL_COMPARE" == "differ" ]; then
				echo -e "\t\tPASS | [${LOCAL_OMAPCONF_DOMAIN[$index]}] domain scaled"
				echo -e "\t\t     | Previous value: $val1"
				echo -e "\t\t     | Curent value  : $val2\n"
				LOCAL_FLAG_DIFFER=1
			fi
		fi
	done
	echo -e "\n-------------------------------------------------------------------\n\n"
	if [[ "$LOCAL_COMPARE" == "differ" && $LOCAL_FLAG_DIFFER -eq 0 ]]; then
		echo ""
		echo " #########################################################################"
		echo " #                                                                       #"
		echo " #   ERROR: None of the [$LOCAL_DOMAIN] domains scaled during the test  "
		echo " #                                                                       #"
		echo " #########################################################################"
		echo ""
		LOCAL_ERROR=1
	fi
	if [[ "$LOCAL_COMPARE" == "match" && $LOCAL_FLAG_DOMAIN_CHANGE -eq 0 ]]; then
		echo ""
		echo " #########################################################################"
		echo " #                                                                       #"
		echo " #          PASS: All the [$LOCAL_DOMAIN] domains values match          "
		echo " #                                                                       #"
		echo " #########################################################################"
		echo ""
	fi
	rm $LOCAL_TMP1 $LOCAL_TMP2
}

# Prints a message with a specific format
# @ Function  : showInfo
# @ Parameters: <message to display>
# @ Return    : None
showInfo() {
	echo -e "\n\n--------------------------------------------------------------\n"
	messages=( "$@" )
	for index in ${!messages[@]}; do
		echo -e "\t${messages[$index]}"
	done
	echo -e "\n--------------------------------------------------------------\n\n"
}

# Verify LOCAL_ERROR flag
# if flag is set to '1' exit the script and register the failure
# The message parameter helps to debug the script
# @ Function  : verifyErrorFlag
# @ Parameters: <debug message>
# @ Return    : None
verifyErrorFlag() {
	debug_message=$1
	if [ $LOCAL_ERROR -eq 1 ]; then
		handlerError.sh "log" "1" "halt" "handlerOmapconf.sh"
		handlerDebugFileSystem.sh "umount"
		showInfo "DEBUG: LOCAL ERROR DETECTED:" "$debug_message" 1>&2
		exit $LOCAL_ERROR
	fi
}


# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
        exit 1
fi
handlerDebugFileSystem.sh "mount"

# Verify script usage and parameters

if [[ $LOCAL_OPERATION == "run" && $# -eq 2  ]]; then
	checkParamInArray $LOCAL_DOMAIN LOCAL_OMAPCONF_DOMAINS
	verifyErrorFlag "checkParamInArray(): verify 'run' parameter"
elif [[ $LOCAL_OPERATION == "compare" && $# -eq 5 ]]; then
	checkParamInArray $LOCAL_DOMAIN LOCAL_OMAPCONF_DOMAINS
	verifyErrorFlag "checkParamInArray(): verify 'compare' parameter"
	verifyFiles $LOCAL_FILE1 $LOCAL_FILE2
	verifyErrorFlag "verifyFiles(): Verifying log files"
	if [ `echo "match differ" | grep -wc "$LOCAL_COMPARE"` -ne 1 ]; then
		showInfo "ERROR: Provide a correct compare parameter" \
			 "Try - "match" or "differ"" 1>&2
		LOCAL_ERROR=1
		verifyErrorFlag "generalUsage(): Verifying [match|differ] parameters"
	fi
else
	generalUsage
	verifyErrorFlag "generalUsage(): Verifying log files"
fi

# Check if omapconf tool is available
if [[ ! -f $LOCAL_OMAPCONF ]]; then
	showInfo "FATAL: omapconf tool is not available at $UTILS_DIR_BIN" 1>&2
	LOCAL_ERROR=1
	verifyErrorFlag "Verify that omapconf tool is available"
fi

# Execute OMAPCONF operations

case $LOCAL_DOMAIN in
"FREQUENCY")
	LOCAL_OMAPCONF_EXECUTE="opp"
	LOCAL_OMAPCONF_DOMAIN=("${LOCAL_FREQ_DOMAINS[@]}")
	LOCAL_OUTPUT_COLUMN=5
	;;
"CLOCKS")
	LOCAL_OMAPCONF_EXECUTE="audit clkspeed"
	LOCAL_OMAPCONF_DOMAIN=("${LOCAL_CLOCK_DOMAINS[@]}")
	LOCAL_OUTPUT_COLUMN=6
	;;
"VOLTAGE")
	LOCAL_OMAPCONF_EXECUTE="opp"
	LOCAL_OMAPCONF_DOMAIN=("${LOCAL_VOLT_DOMAINS[@]}")
	LOCAL_OUTPUT_COLUMN=6
	;;
*)
	generalUsage
	verifyErrorFlag "generalUsage(): Verifying LOCAL_DOMAIN"
	;;
esac

case $LOCAL_OPERATION in
"run")
	# Every time test_runner is called to execute a command, there is an
	# increment in voltage and frequency. A small sleep will allow the system
	# to return to previous state. This will allow us to take valid measurements.
	sleep 4; $LOCAL_OMAPCONF $LOCAL_OMAPCONF_EXECUTE
	showInfo "DEBUG: Running 'omapconf $LOCAL_OMAPCONF_EXECUTE'"
	;;
"compare")
	compareOmapconfResults
	verifyErrorFlag "compareOmapconfResults()"
	;;
*)
	generalUsage
	verifyErrorFlag "generalUsage(): Verifying LOCAL_OPERATION"
	;;
esac

handlerDebugFileSystem.sh "umount"
exit $LOCAL_ERROR

# End of file
