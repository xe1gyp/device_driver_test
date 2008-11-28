README.txt (Designed in 80x120 vi screen)
---------
DATE: 26/04/2006

External Applications required:
.	madplay (http://www.underbit.com/products/mad/)
.	aumix (http://freshmeat.net/projects/aumix/)
.	sox (http://sox.sourceforge.net/)
Filesystem requires to have libmath

COMPILATION:
1)modify Makefile 
	* define GSAMPLE_SHIFT to enable support for Bitshifting in <PG2 triton1 (not TWL4030)
	* change CC to point to cross compiler to use
2) make

CLEANUP:
1) make clean

DETAILED DESCRIPTION:
=====================
   0.                   Audio Test Automation Framework

   Executing the combinations of the test cases, running them is impractical for a single human
   considering the limited time that is usually available to run them. This is the major motivation for the
   automation of these tests. The automated framework for OMAP OSS Audio driver is based on Linux Test Project
   (http://ltp.sourceforge.net/). The LTP utility "pan" is used as the wrapper to run the test and decode the result
   of the test. See http://www.penguin-soft.com/penguin/man/1/ltp-pan.html for more details on pan.

           +----------------------------------------------------------------------------------+
           |                                                                                  |
           |                                             Executes +------------------+        |
           | +-----------------+     \                         +->| Test Application |        |
           | | Scenario File 1 |     | I/P +----------------+ /   +------------------+        |
           | | +---------------+-+   |->   | Test_Runner.sh |                                 |
           | | |Scenario File 2  |   |     +----------------+                                 |
           | | +-----------------+-+ /               \                                        |
           | | | Scenario File n   |                 | Generates                              |
           | | |                   |                 +--------> +-----------+                 |
           | | |   Test1           |                            |Output File|                 |
           | | |     Test2         |                            | +----------+                |
           | +-|       ..          |                            | | Log File |                |
           |   |         Testn     |                            | |          |                |
           |   +-------------------+                            +-|          |                |
           |                                                      +----------+                |
           |                                                                                  |
           +----------------------------------------------------------------------------------+

   Figure 2                   Audio Test Automation Framework
   The description of the tests is described in a scenario file. Various scenario files are collated into a single
   test run. Each test targets testing a specific test sequence and number such test sequences may complete a single
   test case. The following sections describe the same in detail.

   1.              The Scenario File

   The basic concept of pan is that the commands to be executed are stored in a command file, these command files
   contain entries explaining the tag of the test and the test itself. If the test commands returns 0 ($? == 0), then
   the test is assumed to be a pass, else it is denoted as failed. Example:

   # Scenario: L_DD_AUDIO_0001

   #-- Devfs checks

   # Check if the fs device exists

   0001  cat /proc/devices | grep sound || exit 1

    

   # Check for basic devfs device

   0002a test -c /dev/sound/dsp && test -c /dev/sound/mixer || exit 1

   # Check for filesystem devfs device

   0002b test -c /dev/dsp && test -c /dev/mixer || exit 1

   A line that starts with `#' is considered a comment. The test tag and the test case is separated by at least a
   single space.

   The file name has the scenario ID and each file contains the test ID with the command sequence that returns 0 if
   the test passed, else it returns 0.

   A test case id within the scenario file _EXT extended test which may be either regression or consumes huge
   duration of time.

   2.              The Test Runner

   The Test runner (test_runner.sh) is the heart of the automated system. It collates all scenario files, removes the
   comments, collates the data in a format that pan understands and then passes it to pan. Pressing Ctrl+c during the
   test stops and cleans up the temporary files.

   Usage: ./test_runner.sh [-z] [-h] [-v] [-d TESTDIR] [-o OUTPUTFILE]

                   [-l LOGFILE] [-n DURATION ] [-t TMPDIR] SCENARIO_NAMES..

   Table 4                 Options of Test Runner

+----------------------------------------------------------------------------------------------------------------------+
|      SL.No      |    Option    |                                     Description                                     |
|-----------------+--------------+-------------------------------------------------------------------------------------|
|1.               |-d TESTDIR    |Run LTP to test the file-system mounted here. At the end of test, the test directory |
|                 |              |gets cleaned out                                                                     |
|-----------------+--------------+-------------------------------------------------------------------------------------|
|2.               |-s TC_SCENARIO|Test scenario files are located here.                                                |
|-----------------+--------------+-------------------------------------------------------------------------------------|
|3.               |-o OUTPUTFILE |Redirect test output to a file.                                                      |
|-----------------+--------------+-------------------------------------------------------------------------------------|
|4.               |-p            |Human readable format log files (default=OFF)                                        |
|-----------------+--------------+-------------------------------------------------------------------------------------|
|5.               |-z            |Don't Merge the Scenario Name with test case id to create final test case id in the  |
|                 |              |report. (default=OFF)                                                                |
|-----------------+--------------+-------------------------------------------------------------------------------------|
|6.               |-E            |Run Extended Test cases also (The test cases with _EXT will also get executed along  |
|                 |              |with the rest. (default=OFF)                                                         |
|-----------------+--------------+-------------------------------------------------------------------------------------|
|7.               |-l LOGFILE    |Log results of test in a log-file.                                                   |
|-----------------+--------------+-------------------------------------------------------------------------------------|
|8.               |-t TMPDIR     |Run LTP using temporary directory.                                                   |
|-----------------+--------------+-------------------------------------------------------------------------------------|
|                 |              |(This is a pan dependent option and may not be present on some pan variants).Execute |
|                 |              |the test suite for given duration. Examples:                                         |
|                 |              |                                                                                     |
|                 |              |                      -n 60s = 60 seconds                                            |
|9.               |-n DURATION   |                                                                                     |
|                 |              |                      -n 45m = 45 minutes                                            |
|                 |              |                                                                                     |
|                 |              |                      -n 24h = 24 hours                                              |
|                 |              |                                                                                     |
|                 |              |                      -n 2d  = 2 days                                                |
|-----------------+--------------+-------------------------------------------------------------------------------------|
|10.              |-v            |Print more verbose output to screen. (default=OFF)                                   |
|-----------------+--------------+-------------------------------------------------------------------------------------|
|11.              |-q            |Quiet Mode - No messages from this script. no info too. (default=OFF)                |
|-----------------+--------------+-------------------------------------------------------------------------------------|
|12.              |-h            |Gives the help screen                                                                |
|-----------------+--------------+-------------------------------------------------------------------------------------|
|13.              |SCENARIO_NAMES|List of scenarios to test.                                                           |
+----------------------------------------------------------------------------------------------------------------------+

   3.              The Test Applications

   The scenario files that contain the test cases can use the following programs for each of the test cases:

   Table 5                 Audio test applications

+----------------------------------------------------------------------------------------------------------------------+
|     SL. No.     |     Application     |                                 Description                                  |
|-----------------+---------------------+------------------------------------------------------------------------------|
|1.               |t2_src_changer       |Sends ioctls to select input and output sources for the TWL4030 codec.        |
|-----------------+---------------------+------------------------------------------------------------------------------|
|                 |                     |Generates sinewave PCM data in 16/24/m/stereo at a suggested sampling rate for|
|2.               |sineGen, sineGen_i386|a requested audio frequency (e.g. 1khz audio sinewave for 44.1khz sampling    |
|                 |                     |rate). (NOTE: the i386 variant is for running on the host which compiles the  |
|                 |                     |code).                                                                        |
|-----------------+---------------------+------------------------------------------------------------------------------|
|                 |                     |This application converts a binary PCM file (such as that recorded/a sinewave |
|3.               |graph_helper         |pcm data) into an output understandable by gnuplot. This allows us to plot the|
|                 |                     |captured / generated data.                                                    |
|-----------------+---------------------+------------------------------------------------------------------------------|
|4.               |dspopen              |Opens a given device and closes it after a short duration of time             |
|-----------------+---------------------+------------------------------------------------------------------------------|
|5.               |dspget               |Gets dsp device parameters such as sample rate, mono/stereo/16/24 etc.        |
|-----------------+---------------------+------------------------------------------------------------------------------|
|6.               |dspset               |Sets dsp device parameters such as sample rate, mono/stereo/16/24 etc.        |
|-----------------+---------------------+------------------------------------------------------------------------------|
|7.               |mixer                |This operates on the mixer device to allow us to set the gain of the supported|
|                 |                     |devices.                                                                      |
|-----------------+---------------------+------------------------------------------------------------------------------|
|8.               |mixerinfo            |Retrieves the mixer settings.                                                 |
|-----------------+---------------------+------------------------------------------------------------------------------|
|9.               |player               |Plays a PCM data on the dsp device.                                           |
|-----------------+---------------------+------------------------------------------------------------------------------|
|10.              |recorder             |Records PCM data from a dsp device and captures it to a file.                 |
|-----------------+---------------------+------------------------------------------------------------------------------|
|11.              |fullDup              |Plays and records PCM data from/to a dsp device.                              |
|-----------------+---------------------+------------------------------------------------------------------------------|
|12.              |dpm_test             |The power management shell script meant to be used for testing scale/suspend  |
|                 |                     |functionality                                                                 |
|-----------------+---------------------+------------------------------------------------------------------------------|
|                 |include.common       |These are files which are wrappers to generate various combinations of play   |
|13.              |play_wrapper.sh      |and record and full duplex operations. (NOTE: the include.common contains the |
|                 |record_wrapper.sh    |common parameters used by each of the wrapper scripts.)                       |
|                 |fd_wrapper.sh        |                                                                              |
|-----------------+---------------------+------------------------------------------------------------------------------|
|14.              |gen_SinFFiles.sh     |This is a wrapper script to generate all combination of sine-wave in the form |
|                 |                     |of raw PCM data.                                                              |
+----------------------------------------------------------------------------------------------------------------------+

    

   4.              The Test-suite Organization

   The test suite is organized as follows:

   All application C source code is located here

   |-- lib <- libraries and dsp* and mixer* applications
   |   `-- test_log <- test log code
   |-- pan <- pan from LTP
   |   `-- cgi
   `-- testsuite <- The base test suite dir, contains test_runner.sh
       |-- bin <- all compiled binaries stored here.
       |-- mod <- place to store kernel modules
       |-- quickie_sc <- Quick scripts to run some basic sanity tests
       |-- raw_sin <- All sample rate sin PCM data is stored here
       |-- scenarios <- All scenario files are located here
       |-- scripts <- wrapper script location
       |-- test <- test directory
       `-- tmp <- temporary directory

   FILE          |   DESCRIPTION
   --------------+---------------
   Makefile      | Makefile for compilation of all test cases
   player.c      | Play an PCM/RAW file
   recorder.c    | Records data to a RAW file
   fullDupTest.c | Full duplex supported playback and record program.
   volumePlay.c  | Plays an PCM/RAW file by varying the volume after every read
   evtest.c      | event dumper - useful for pulling touchscreen/keypad data
   timemeasure.c | time measurement library(performance data)
   t2_src_changer.c | Change the input/output source on TRITON2
   mixer.c       | mixer app
   mixerinfo.c  | mixerinfo app
   sineGen.c     | sine wave generator
   graph_helper.c | convert raw data to gnuplot readable format

<EOD>
