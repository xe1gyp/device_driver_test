#################################################
# 1. Build testsuite using commands:		#
#################################################
	cd device_driver_test/
	The following variables are needed for the
	testsuite build system.
	They should be either exported to the environment
	before starting 'make' or 
	passed as parameters to make.
	Sane defaults are assumed when these
	parameters are not specified

Method 1
--------
	export CROSS_COMPILE=arm-none-linux-gnueabi-
	export KDIR=[PATH-TO-KERNEL]
	export HOST=arm-none-linux-gnueabi
	# driver_x=The driver for which the testsuite is to be built
	# Use "all" for building the entire testsuite
	export TESTSUITES="driver_x"
	export TESTROOT=<output dir>
	make

 The testsuite will be created in the location specified by <output dir>

Method 2
--------
	make KDIR=[Path-To-Kernel] TESTSUITES="all"

 The testsuite will be created in the `device_driver_test/build` directory


#################################################
# 2. Running test scenarios			#
#################################################
# These are to be run using the output-directory specified above
# Here fb_testsuites is the output directory

## 2.1 Procedure to run the scripts automatically is:

	cd /fb_testsuites/framebuffer/scripts
	./run_all.sh

# This is applicable for only few drivers #

## 2.2 Procedure to run the scenarios manually
## without using user intervention:

	cd /fb_testsuites/framebuffer/scripts

# and then run ./test_runner.sh -S scenario_name
# Example :
./test_runner.sh -S L_DD_FB_0001

# 2.3 Procedure to run the scenarios manually
# entering based on what you see,
# ie whether the test case passed or failed is:

cd /fb_testsuites/framebuffer/scripts

 ./test_runner.sh  scenario_name
#Example
./test_runner.sh -S L_DD_FB_0001 
