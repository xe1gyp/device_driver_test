C:ompile Instructions
--------------------
1. Clean the environment.
   Perform 'make clean'. This will erase all files and directories created during previous compilation.
2. Install.
   Perform 'make CROSS_COMPILE='compiler prefix''
   For 8.x Program
      * CROSS_COMPILE=arm_v6_vfp_le-
   For 12.x Program
      * CROSS_COMPILE=arm-none-linux-gnueabi-
   For 18.x Program
      * CROSS_COMPILE=arm-none-linux-gnueabi-


Testsuite execution
-------------------
1. Go to 'scripts' subdirectory inside the testsuite directory.
2. Execute the main script 'test_runner.sh -p 'scenario_id'.
   * The scenarios IDs can be found under script/scenarios/
3. Results will be reported showing Scenario-TestCase IDs and the result of each test.
   * 0 - Successful
   * Any other value different of 0 - Failure


Requirements
------------
The file system on which the testsuite is going to be executed needs to have the following utilities:
* sh
* bc
* bash
* ALSA utilities
* OMX applications


Notes
-----
