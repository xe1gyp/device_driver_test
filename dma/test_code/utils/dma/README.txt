README.txt
Date: Jan 4, 2006
Author: Nishanth Menon <x0nishan@ti.com>
----------
Legal Stuff
-----------
Copyright (C) 2004-2006 Texas Instruments, Inc.

This package is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.

THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
--------------
Description:
This Test Suite is meant to test the dma driver to a detailed extent
DMA Library R2.xx for Montavista® Linux® 2.4.20/2.6 has no user APIs exposed, 
hence to test the same would require a test driver for the same which provides user
interface to query test results. This test driver has its origins on a test
driver originally by Tony Lindgren available here:
http://muru.com/linux/omap/patches/patch-2.6.1-omap-dma-test-3.bz2 
---------------------------------------------------------------------
File Description:
-------------
Makefile - compilation environment capable of handling 2.4 and 2.6 kernels
README.txt - this file
dma_buf_logic.c   - This is common buffer handling logic used by all test
                    drivers
dma_stat.c        - The Statistics handling logic which also provides the proc
                    interface to query the info out
dmatest.h         - Shared test driver structures
dmadynamic_chain.c- Dynamic Chaining test driver
dmastatic_chain.c - Static Chaining test driver
dmaunlnk.c        - Un chained dma transfer test driver
runAllDmaTests.sh - Wrapper test suite.  Read the header in this script to see
                    the tests captured

Compilation instructions:
-------------------------
Open the Makefile and modify the KDIR variable to point to the kernel for
which the test is to be compiled for.
NOTE: The test suite at the time of writing is capable of testing only the new
dma chaining APIs. Support for the old apis have been removed.

make - compiles kernel modules for 2.4 kernel
make 26 - compiles the kernel modules for the 2.6 kernel
make clean - cleans up the binaries

Running the test suite:
-----------------------
./runAllDmaTests.sh [TEST_NUMS]
    Where the TEST_NUMS are the test numbers 1 2 3...
    enumerate all the tests that you would like to run in a certain run.

    Read the documentation within runAllDmaTests for more details on the tests
    themselves

Proc filesystem:
----------------
the files are made available based on module names- /proc/drivers/dmatest
prefix is used.
