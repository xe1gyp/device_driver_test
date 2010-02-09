#################################################
# 1. Build testsuite using commands:		#
#################################################
	cd device_driver_test/
	export CROSS_COMPILE=arm-none-linux-gnueabi-
	export KDIR=[PATH-TO-KERNEL]
	export HOST=arm-none-linux-gnueabi
	export TESTSUITES="driver_x"

	# driver_x=The driver for which the testsuite is to be built
	# Use "all" for building the entire testsuite

	./buildTestSuites.sh <output directory>

# The testsuite will be created in the `<outputdirectory>/testsuite` directory

#################################################
# 2. Building test bianries:			#
#################################################

# You might have to go inside the respective directories
# for building test binaries for individual driver
#
#Please use following commands:
	cd driver_x
	make clean
	make all

#################################################
# 3. Running test scenarios			#
#################################################
# These are to be run using the output-directory specified above
# Here fb_testsuites is the output directory

## 3.1 Procedure to run the scripts automatically is:

	cd /fb_testsuites/framebuffer/scripts
	./run_all.sh

# This is applicable for only few drivers #

## 3.2 Procedure to run the scenarios manually
## without using user intervention:

	cd /fb_testsuites/framebuffer/scripts

# and then run ./test_runner.sh -S scenario_name
# Example :
./test_runner.sh -S L_DD_FB_0001

# 3.3 Procedure to run the scenarios manually
# entering based on what you see,
# ie whether the test case passed or failed is:

cd /fb_testsuites/framebuffer/scripts

# ./test_runner.sh  scenario_name


#Example
./test_runner.sh -S L_DD_FB_0001 But as there are

