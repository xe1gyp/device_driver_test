/*
    Mark Hattarki <hattarki@ti.com>
    
    Texas Instraments, All Rights Reserved
    
    Desc:
	This is the logging library for Tests
	written in C. The python lib should be
	conscidered the reference lib.
	
	
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <errno.h>

#include "list.h"
#include "testlog.h"

#define STRSIZE 256
#define STDOUTFD  1

/* functions */

int
mywrite(int fd, const char *str)
{
    int cur = 0, ret = 0;
    int len = strlen(str);
    while (cur < (len)) {
	ret = write(fd, &str[cur], len - cur);
	if (ret < 0) 
	    return -1;
	else
	    cur += ret;
    }
    return len;
}

int 
file_not_exist(const char *fname)
{
    struct stat statd;
    int ret;
    assert(fname != NULL);
    ret = stat(fname, &statd);
    if ((ret != 0) && (errno == ENOENT))
	return 1;
    else
	return 0;
}

int
rename_file(const char *fname)
{
    struct stat statd;
    int ret;
    char buff[STRSIZE];
    assert(fname != NULL);
    if ((ret = stat(fname, &statd)) != 0)
	return errno;
    snprintf(buff, STRSIZE, "%s_%d", fname, (int) statd.st_mtime);
    if ((rename(fname, buff)) != 0)
	return errno;
    else
	return 0;
}

testlog*
test_create(void)
{
    testlog *p = (testlog *) malloc(sizeof(testlog));
    if (p == NULL) return p;
    bzero(p, sizeof(testlog));
    p->loglist = list_create();
    p->passes = 0;   /* redundant, but whatever */
    p->failures = 0; /* same */
    p->logging = 0;
    p->errors = 0;
    addfilebytype(p, STDOUTFD, VERBOSE);
    return p;
}

int 
addfilebytype(testlog *log,
	      int fd,
	      int dlevel)
{
    int ret = 0;
    lfile *p = (lfile *) malloc(sizeof(lfile));
    if (p == NULL)
	return -1;
    assert (log->logging == 0);
    p->fd = fd;
    p->loglevel = dlevel;
    ret = list_insert(log->loglist, p);
    if (ret != 1) 
	return -1;
    else
	return 1;
}

int
addfilebyname(testlog *l,
	      const char *fname,
	      int dlevel)
{

    int ret;
    int fd;
    assert((l->logging == 0) && (fname != NULL));
/*
 Vijaya: ugly hack to make it work with command line for loggin to 
 the same file 
*/
#if 0 
	 
    if (!(file_not_exist(fname))) {
	if ((ret=(rename_file(fname))) != 0) {
		printf("RETUNIG\n");
	    return ret; 
	}
    }
#endif
    fd = open(fname,
	      O_WRONLY | O_CREAT |O_APPEND,
	      S_IRUSR | S_IWUSR | S_IRGRP |
	      S_IWGRP| S_IROTH | S_IWOTH);
    /* the file perms in the above are not set if
       the file already exists. To be safe, we call
       fchmod. Taking them out of open doesn't mean
       much
    */
    if (fd < 0) return fd;
    ret = fchmod(fd, S_IRUSR | S_IWUSR | S_IRGRP | 
		 S_IWGRP | S_IROTH | S_IWOTH);
    if (ret < 0) return ret;
    else {
		lseek (fd, SEEK_END, 0);	
		return addfilebytype(l, fd, dlevel);
	}

}

int
log(testlog *l,
    msgtype type,
    const char *msg,
    int dlevel)
{
    char tempstr[STRSIZE];
    time_t timep;
    char *timestr;
    lfile *lfp = NULL;
    assert(l->logging);
    time(&timep);
    timestr = (char *) ctime(&timep);
    /* add consistancy checks */
    list_reset(l->loglist);
    while ((lfp = (lfile*) list_getnext(l->loglist)) != NULL) {
	if (dlevel <= lfp->loglevel) {
	    write(lfp->fd, timestr, strlen(timestr) - 1);
	    snprintf(tempstr, STRSIZE, "%c", DLIMITOR);
	    mywrite(lfp->fd, tempstr);
	    switch (type) {
	    case ERRORL : mywrite(lfp->fd, "ERROR");
		          break;
	    case INFO:    mywrite(lfp->fd, "INFO");
		          break;
	    case PASS:    mywrite(lfp->fd, "PASS");
		          break;
	    case FAIL:    mywrite(lfp->fd, "FAIL");
		          break;
	    case WARN:    mywrite(lfp->fd, "WARN");
		          break;
	    case START:   mywrite(lfp->fd, "START");
		          break;
	    case CASE_START:
			  mywrite(lfp->fd, "CASE_START");
			  break;
	    case CASE_PASS:
	                  mywrite(lfp->fd, "CASE_PASS");
			  break;
	    case CASE_FAIL:
	                  mywrite(lfp->fd, "CASE_FAIL");
			  break;
	    case SYSINFO: mywrite(lfp->fd, "SYSINFO");
	                  break;
	    case SCEN:    mywrite(lfp->fd, "SCEN");
	                  break;
	    case DESC:    mywrite(lfp->fd, "DESC");
	                  break;
	    case CASE_ID: mywrite(lfp->fd, "CASE_ID");
	                  break;
	    case VALID:   mywrite(lfp->fd, "VALIDATION");
	                  break;
	    case END:     mywrite(lfp->fd, "END");
	                  break;
	    default:      snprintf(tempstr,
				   STRSIZE,
			           "incorrect message type %d",
			           type);
		          log(l, WARN, tempstr, WARNING);
	    }
	    snprintf(tempstr, STRSIZE, "%c%s\n", DLIMITOR, msg);
	    mywrite(lfp->fd, tempstr);
	}
    }
    return 1;
}

int
test_init(testlog *l,
	  const char *name)
{
    struct utsname udata;
    int ret;
    assert((l != NULL) && (name != NULL));
    l->logging = 1;
    log(l, START, name, INFORMATION);
    if ((ret = uname(&udata) <0))
	test_error(l, "uname call failed");
    test_sysinfo_f(l, "system name: %s", udata.sysname);
    test_sysinfo_f(l, "node name: %s", udata.nodename);
    test_sysinfo_f(l, "release: %s", udata.release);
    test_sysinfo_f(l, "version: %s", udata.version);
    test_sysinfo_f(l, "machine: %s", udata.version);
    return 1;
}

int
test_sysinfo(testlog *l,
	  const char *msg)
{
    assert((l != NULL) && (msg != NULL)
	   && (l->logging));
    return log(l, SYSINFO, msg, ERROR);
}

int
test_warn(testlog *l,
	  const char *msg)
{
    assert((l != NULL) && (msg != NULL)
	   && (l->logging));
    return log(l, WARN, msg, WARNING);
}

int
case_start(testlog *l,
	  const char *name)
{
    assert( (l != NULL) && (name != NULL) 
	    && (l->logging));
    l->case_start = 1;
    return log(l, CASE_START, name, ERROR);
}

int
case_pass(testlog *l,
	  const char *msg)
{
    assert((l != NULL) && (msg != NULL)
	   && (l->logging));
    assert(l->case_start); /* case start must be called */
    l->passes++;
    l->case_start = 0;
    return log(l, CASE_PASS, msg, INFORMATION);
}

int
case_fail(testlog *l,
	  const char *msg)
{
    assert((l != NULL) && (msg != NULL) 
	   && (l->logging));
    assert(l->case_start);
    l->case_start = 0;
    l->failures++;
    return log(l, CASE_FAIL, msg, ERROR);
}

int
test_info(testlog *l,
	  const char *msg)
{
    assert((l != NULL) && (msg != NULL) 
	   && (l->logging));
    return log(l, INFO, msg, INFORMATION);
}

int
case_description(testlog *l,
		 const char *str)
{
    assert((l != NULL) && (str != NULL) 
	   && (l->logging));
    return log(l, DESC, str, INFORMATION);
}

int
case_scenario(testlog *l,
	      const char *scenario)
{
  assert((l != NULL) && (scenario != NULL) 
	 && (l->logging));
  return log(l, SCEN, scenario, INFORMATION);
}

int
case_validation(testlog *l,
		const char *vstr)
{
  assert((l != NULL) && (vstr != NULL) 
	 && (l->logging));
  return log(l, VALID, vstr, INFORMATION);
}

int
test_error(testlog *l,
	   const char *msg)
{
    assert((l != NULL) && (msg != NULL) 
	   && (l->logging));
    l->errors++;
    return log(l, ERRORL, msg, ERROR);
}

void
test_error_exit(testlog *l,
		const char *msg)
{
    test_error(l, msg);  
    /* no need to check return val, exiting anyway */
    test_exit(l, msg);
}

void
test_close(testlog *l)
{
    lfile *lfp = NULL;
    assert(l != NULL);
    list_reset(l->loglist);
    lfp = (lfile*) list_getnext(l->loglist);
    while (lfp != NULL) {
	close(lfp->fd);
	lfp = (lfile*) list_getnext(l->loglist);
    }
}

void
test_exit(testlog *l,
	  const char *msg)
{
    /*at this point, ignore warnings, log what
     ya can */
    char lmsg [STRSIZE];
    assert(l != NULL);
    snprintf((char*) &lmsg, STRSIZE - 1, "CASES PASSED : %d", l->passes);
    log(l, INFO, lmsg, ERROR);
    snprintf((char*) &lmsg, STRSIZE - 1, "CASES FAILED : %d", l->failures);
    log(l, INFO, lmsg, ERROR);
    snprintf((char*) &lmsg, STRSIZE - 1, "ERRORS : %d", l->errors);
    log(l, INFO, lmsg, ERROR);
    if ((l->failures > 0) || (l->errors > 0)) 
	log(l, FAIL, "TESTFAILED", ERROR);
    else 
	log(l, PASS, "TESTPASSED", ERROR);
    log(l, END, msg, INFORMATION);
    test_close(l);
    exit(l->failures + l->errors);
}

void
test_pass(testlog *l,
	  const char *msg)
{
    assert(l != NULL);
    test_exit(l, msg);
}

void
test_fail(testlog *l,
	  const char *msg)
{
    if (l->case_start) 
	case_fail(l,msg);
    test_exit(l, msg);
}

