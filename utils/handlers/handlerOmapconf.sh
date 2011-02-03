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

command=$1
domain=$2
log_one=$3
log_two=$4
compare_flag=$5
var1="value1.log"
var2="value2.log'"
omapconf_params=()
omapconf_dm=()
output_column=0
flag_differ=0
flag_val_change=0
error_val=0
omapconf_tool="$UTILS_DIR_BIN/omapconf"

# =============================================================================
# OMAPCONF test domains
# =============================================================================

omapconf_audit=( VOLTAGE FREQUENCY CLOCKS )

clock_domains=( WKUP_L4_ICLK2 WKUP_32K_GFCLK GPT1_FCLK WKUP_32K_GFCLK \
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

freq_domains=( "MPU" "TESLA" "IVAHD" "ABE" "L3" "DMM/EMIF" "DLL" \
                     "DDRPHY" "L4" "SGX" "FDIF" "DUCATI" "DSS" "HSI" )

voltage_domains=( VDD_MPU VDD_IVA VDD_CORE )

# =============================================================================
# Functions
# =============================================================================

# Display the script usage
# @ Function: generalUsage
# @ parameters: None
# @ Return: Error flag value
usage() {
	echo -e "\n################# handlerOmapconf #################\n"
	echo -e "  Try - handlerOmapconf.sh {run|compare} <OPTIONS>\n"
	echo -e "  Example with "run" command:"
	echo -e "  ./handlerOmapconf run <function>\n"
	echo -e "  Example with "compare" command:"
	echo -e "  ./handlerOmapconf compare <function> <LOG1> <LOG2>" \
		"\n\t\t    {match|differ}\n"
	echo -e "  Where:\n"
	echo -e "  <function> can be:"
	for index in ${!omapconf_audit[*]}; do
		echo "	- ${omapconf_audit[$index]}"
	done
	echo -e "  {match|differ} are comparison flags"
	echo -e "\n################# handlerOmapconf #################\n"
	error_val=1
}

# This function checks if a parameter
# is declared inside of an array
# @ Function: checkParamInArray
# @ Parameters: <parameter> <array>
# @ Return: Error flag value
checkParamInArray() {
	param=$1
	array_name=$2
	# Saving all the arrays values into a temporal variable
	eval "eval 'echo "\${$array_name[@]}"'" > temp
	if [ ! `cat temp | grep -wc $param` -gt 0 ]; then
		echo -e "\nERROR: $param is not declared in $array_name array\n"
		error_val=1
	fi
	rm temp
}

# This function verify is the file(s) given exist
# @ Function: verifyFiles
# @ Parameters: <file1> <file2> <...>
# @ Return: Error flag value
verifyFiles() {
	files=( "$@" )
	for index in ${!files[@]}; do
		if [ ! -f ${files[$index]} ]; then
			showInfo "ERROR: ${files[$index]}  does not exist" 1>&2
			error_val=1
		fi
	done
}

# Compares similar values inside of different omapconf tables
# The functions takes the following predefined variables:
# - OMAPCONF domain array
# - Two logs containing omapconf tables
# - Column number inside of the table
# @ Function: compareOmapconfResults
# @ Parameters: None
# @ Return: Error flag value
compareOmapconfResults() {
	echo -e "\n################# handlerOmapconf #################\n"
	for idx in ${!omapconf_dm[*]}; do
		grep -m 1 -w "${omapconf_dm[$idx]}" $log_one \
				| awk '{printf$'$output_column'}' > $var1
		grep -m 1 -w "${omapconf_dm[$idx]}" $log_two \
				| awk '{printf$'$output_column'}' > $var2
		val1=`cat $var1`
		val2=`cat $var2`
		if [ $val1 != $val2 ]; then
			flag_val_change=1
			if [ "$compare_flag" == "match" ]; then
				echo -e " FAIL | [${omapconf_dm[$idx]}] domain differ"
				echo -e "      | Current  value: $val1"
				echo -e "      | Expected value: $val2\n"
				error_val=1
			elif [ "$compare_flag" == "differ" ]; then
				echo -e " PASS | [${omapconf_dm[$idx]}] domain scaled"
				echo -e "      | Previous value: $val1"
				echo -e "      | Curent value  : $val2\n"
				flag_differ=1
			fi
		fi
	done
	echo -e "\n################# handlerOmapconf #################\n"
	if [[ "$compare_flag" == "differ" && $flag_differ -eq 0 ]]; then
		echo -e "\n################# handlerOmapconf #################\n"
		echo -e " ERROR:None of the [$domain] domains scaled"
		echo -e "\n################# handlerOmapconf #################\n"
		error_val=1
	fi
	if [[ "$compare_flag" == "match" && $flag_val_change -eq 0 ]]; then
		echo -e "\n################# handlerOmapconf #################\n"
		echo -e " PASS: All the [$domain] domains values match"
		echo -e "\n################# handlerOmapconf #################\n"
	fi
	rm $var1 $var2
}

# Prints a message with a specific format
# @ Function: showInfo
# @ Parameters: <message to display>
# @ Return: None
showInfo() {
	messages=( "$@" )
	for index in ${!messages[@]}; do
		echo "[ handlerOmapconf ] ${messages[$index]}"
	done
}

# Verify error_val flag
# if flag is set to '1' exit the script and register the failure
# The message parameter helps to debug the script
# @ Function: verifyErrorFlag
# @ Parameters: <debug message>
# @ Return: None
verifyErrorFlag() {
	debug_message=$1
	if [ $error_val -eq 1 ]; then
		handlerError.sh "log" "1" "halt" "handlerOmapconf.sh"
		handlerDebugFileSystem.sh "umount"
		showInfo "DEBUG: LOCAL ERROR DETECTED:" "$debug_message" 1>&2
		exit $error_val
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

if [[ $command == "run" && $# -eq 2  ]]; then
	checkParamInArray $domain omapconf_audit
	verifyErrorFlag "checkParamInArray(): verify 'run' parameter"
elif [[ $command == "compare" && $# -eq 5 ]]; then
	checkParamInArray $domain omapconf_audit
	verifyErrorFlag "checkParamInArray(): verify 'compare' parameter"
	verifyFiles $log_one $log_two
	verifyErrorFlag "verifyFiles(): Verifying log files"
	if [ `echo "match differ" | grep -wc "$compare_flag"` -ne 1 ]; then
		showInfo "ERROR: Provide a correct compare parameter" \
			 "Try - "match" or "differ"" 1>&2
		error_val=1
		verifyErrorFlag "usage(): Verifying [match|differ] parameters"
	fi
else
	generalUsage
	verifyErrorFlag "usage(): Verifying log files"
fi

# Check if omapconf tool is available
if [[ ! -f $omapconf_tool ]]; then
	showInfo "FATAL: omapconf tool is not available at $UTILS_DIR_BIN" 1>&2
	error_val=1
	verifyErrorFlag "Verify that omapconf tool is available"
fi

# Execute OMAPCONF operations

case $domain in
"FREQUENCY")
	omapconf_params="opp"
	omapconf_dm=("${freq_domains[@]}")
	output_column=5
	;;
"CLOCKS")
	omapconf_params="audit clkspeed"
	omapconf_dm=("${clock_domains[@]}")
	output_column=6
	;;
"VOLTAGE")
	omapconf_params="opp"
	omapconf_dm=("${voltage_domains[@]}")
	output_column=6
	;;
*)
	generalUsage
	verifyErrorFlag "usage(): Verifying domain"
	;;
esac

case $command in
"run")
	# Every time test_runner is called to execute a command
    # there is an increment in voltage and frequency. A small
    # sleep will allow the system to return to previous state.
    # This will allow us to take valid measurements
	sleep 4; $omapconf_tool $omapconf_params
	showInfo "DEBUG: Running 'omapconf $omapconf_params'"
	;;
"compare")
	compareOmapconfResults
	verifyErrorFlag "compareOmapconfResults()"
	;;
*)
	generalUsage
	verifyErrorFlag "usage(): Verifying command"
	;;
esac

handlerDebugFileSystem.sh "umount"
exit $error_val

# End of file
