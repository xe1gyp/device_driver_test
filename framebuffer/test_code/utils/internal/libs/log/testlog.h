/*
    Mark Hattarki <hattarki@ti.com>
    
    Texas Instraments, All Rights Reserved
    
    Desc:
	This is the logging library for Tests
	written in C. The python lib should be
	conscidered the reference lib.
	
	Basically, you create a log instance, with
	the test_create() call. Then use add_file_by_name()
	to add files to the list (by name). You must call
	test_init() when the test is started. Use test_warn()
	test_info() and test_error() for message output. 
	case_pass|fail are used if individual cases within
	the program fail. test_exit(),
	will output the number of failures to the logs, and
	exit the running program, zero if the number of failures
	(the number of times case_fail has been called) is 
	zero, otherwise -1. Not that the test is expected to
	exit via one of the following: test_fail(), test_pass()
	test_exit();
	
	So, you must call*:
	test_create()
	addfilebyname()
	test_init()
	...
	case_start()
	...
	case_pass() or case_fail()
	...
	test_exit() 

	* as it so happens there, there are some macro analogs
	to these functions. For example, if you really don't
	want to format your output, you can use the 
	test_create_f(...) macro instead. This will format
	your output, with your standard printf format and add
	call the coresponding function.
	
    example:
    
    int ret;
    testlog *l = test_create();
    ret = addfilebyname(l, "testout.dat", VERBOSE);
    if (ret <0 ) {
	fprintf(stderr, "could not open logfiles");
	exit(-1);
	}
    test_init(l, "starting");
    test_warn(l, "this is a warning");
    test_info(l, "info message");
    case_start(l, "test_case01");
    case_pass(l, "test case passed");
    case_start(l, "test_case02");
    case_fail(l, "test case failed");
    test_exit(l, "test exit");
    
*/


#ifndef TESTLOG__H
#define TESTLOG__H

#include "list.h"

/* constants and defines */
#ifndef uint
#define uint unsigned int
#endif

#define		DLIMITOR	'|'

#define		ERROR		0
#define 	WARNING 	1
#define		INFORMATION     2
#define		VERBOSE 	3

#define LOG_BUFF_SIZE	       	512 

/* macros */

/* these behave the exact same way as the functions with
   the same name (without the "_f"), except they will
   format your message just like printf/sprintf would.
*/

#define test_init_f(tlog, str, args...) \
    snprintf(tlog->log_buff, LOG_BUFF_SIZE, str, args); \
    test_init(tlog, (const char*) tlog->log_buff)

#define test_info_f(tlog, str, args...) \
    snprintf(tlog->log_buff, LOG_BUFF_SIZE, str, args); \
    test_info(tlog, (const char*) tlog->log_buff)

#define test_sysinfo_f(tlog, str, args...) \
    snprintf(tlog->log_buff, LOG_BUFF_SIZE, str, args); \
    test_sysinfo(tlog, (const char*) tlog->log_buff)

#define case_start_f(tlog, str, args...) \
    snprintf(tlog->log_buff, LOG_BUFF_SIZE, str, args); \
    case_start(tlog, (const char*) tlog->log_buff)    

#define case_pass_f(tlog, str, args...) \
    snprintf(tlog->log_buff, LOG_BUFF_SIZE, str, args); \
    case_pass(tlog, (const char*) tlog->log_buff)
    
#define case_fail_f(tlog, str, args...) \
    snprintf(tlog->log_buff, LOG_BUFF_SIZE, str, args); \
    case_fail(tlog, (const char*) tlog->log_buff)
    
#define test_pass_f(tlog, str, args...) \
    snprintf(tlog->log_buff, LOG_BUFF_SIZE, str, args); \
    test_pass(tlog, (const char*) tlog->log_buff)
    
#define test_fail_f(tlog, str, args...) \
    snprintf(tlog->log_buff, LOG_BUFF_SIZE, str, args); \
    test_fail(tlog, (const char*) tlog->log_buff)

#define test_warn_f(tlog, str, args...) \
    snprintf(tlog->log_buff, LOG_BUFF_SIZE, str, args); \
    test_warn(tlog, (const char*) tlog->log_buff)
    
#define test_error_f(tlog, str, args...) \
    snprintf(tlog->log_buff, LOG_BUFF_SIZE, str, args); \
    test_error(tlog, (const char*) tlog->log_buff)
    
#define test_error_exit_f(tlog, str, args...) \
    snprintf(tlog->log_buff, LOG_BUFF_SIZE, str, args); \
    test_error_exit(tlog, (const char*) tlog->log_buff)
    

/* types */

typedef enum {ERRORL, INFO, PASS, FAIL, WARN, START, END, CASE_START, SYSINFO} msgtype;

typedef struct {
    int fd;
    int loglevel;
} lfile;

typedef struct {
    list *loglist;
    int passes;
    int failures;
    int errors;
    char log_buff[LOG_BUFF_SIZE];
    int logging;    /* has test_init been called */
    int case_start; /* case start must be called before case_pass/fail */
} testlog;

/* functions */

int
addfilebytype( testlog *l, 
	       int fd, 
	       int dlevel);
/* This adds a file to the list of files being
   logged to. This adds by file descriptor
*/

int
addfilebyname( testlog *l, 
	       const char* fname, 
	       int loglevel); 
/* This adds a file to the list of files being
   logged to. This adds by file name
*/

testlog*
test_create (void);
/* create a test log and return a pointer to it.
   Use addfilebyname. etc, to add files.
   This function must be called to creat the log
   structure!
*/

int 
test_init(testlog *l, const char* name);
/* The test starts logging and msg will be printed
   to the logs
   This function must be called before any output
   is logged and and after test_create (and, presumably,
   addfilebyname/addfilebytype)
   
   mame is the name of the test.
*/

int
case_start(testlog *l, const char* name);
/* Call when a case starts... it takes the
   name of a case
*/

int
case_pass(testlog *l, const char* msg);
/* A case in a series of cases, in the overall test
   has passed. Msg is written to the log files
*/

int
case_fail(testlog *l, const char *msg);
/* A case is a series of cases failed, msg will be
   written to the log files.
*/

void
test_close(testlog *l);
/* closes all of the log files */

int
test_sysinfo(testlog *l, const char* msg);
/* for system information */

int
test_info(testlog *l, const char* msg);
/* Log a message with level INFO */

int
log(testlog *l,
    msgtype type,
    const char *msg,
    int dlevel);
/* Log a message at the given log level 
   NOTE: Direct use of this function is not
   prohibited, but it is discouraged. Use
   one of the other functions to log
*/


int
start(testlog *l, const char *msg);
/* The test is starting. This is called
   by test init, so don't use
*/

void
test_exit(testlog *l, const char *msg);
/* The test case is exiting. This will
   log the number of test cases that passed
   and the number that failed, in addition
   to calling exit(). If failures exist, this
   will exit non-zero
*/

void
test_fail(testlog *l, const char *msg);
/* The test, in it's entiretly, has reached a
   point where it will no longer continue and
   has failed. This function will exit with a
   failure
*/

void
test_pass(testlog *l, const char *msg);
/* The test has passed and will no longer
   continue. This will log a pass and exit
   with 0.
*/

int
test_warn(testlog *l, const char *msg);
/* log a warning.
*/

int
test_error(testlog *l, const char *msg);
/* log errors that happen during a test
   These are conscidered to be errors not
   related to the tests themselves. This
   does not exit the test, so test_exit()
   still needs to be called. This kinda goes
   without saying, but the msg should allow
   the test executor to figure out the
   problem, if possible.
*/

void
test_error_exit(testlog *l, const char *msg);
/* This is a shortcut for:
   test_error()
   test_exit()
   
   See the docs for these two functions to see
   what they do (yeah, comment abstraction!)
*/

#endif

