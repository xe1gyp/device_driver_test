/*********************************************************************************
Name of the file: fb_user_tests.h

Location of the file: PET/src/test/os/device_drivers/framebuffer/fb_user_tests.h

Created On: 9/28/03
Change Log:
09/28/03 - Prathibha - Adopted for Framebuffer user space testing
Author - Prathibha Tammana - prathibha@ti.com
**********************************************************************/

#ifndef __FBUSER_TESTS_H
#define __FBUSER_TESTS_H

#define DEV_FILE        "/dev/fb0"

#define FBIOBADIOCTL	-30
#define TST_BAD_IOCTL_CMD		1
#define TST_SCREENINFO			2
#define TST_CMAP			3
#define TST_DATA			4
#define BUF_LEN		50

/*
 *  Define new ioctls for the tests - need to define only those used in
 *  error testing - all others are part of other sound related header files
 */


#define TEST_NAME	"fb_tests"
#define NOT_ROOT_FAIL	-1
#define DEV_OPEN_FAIL	-2


/*
 * Function prototypes from fb_user_tests.c
 */
int call_test_open(int);
int call_test_ioctl(int, int);
int call_test_write(int, int);

// Undefine TESTLOG_LIBRARY_AVAILABLE if you don't want to use the test log library
#undef TESTLOG_LIBRARY_AVAILABLE
#define TESTLOG_LIBRARY_AVAILABLE

#ifndef TESTLOG_LIBRARY_AVAILABLE
#define OUT(args...) ({ int ret; ret = printf(args); printf("\n"); ret; });
#define case_pass(x, arg...)	OUT(arg)
#define case_fail(x, arg...)	OUT(arg)
#define case_fail_f(x, arg...)	OUT(arg)
#define case_start(x, arg...)	OUT(arg)
#define test_info_f(x, arg...)	OUT(arg)
#define test_fail(x, arg...)	OUT(arg)
#define test_fail_f(x, arg...)	OUT(arg)
#define test_error_exit_f(x, arg...)	OUT(arg)
#define test_init(x, arg...)	OUT(arg)
#define test_exit(x, arg...)	OUT(arg)
#endif

#endif
