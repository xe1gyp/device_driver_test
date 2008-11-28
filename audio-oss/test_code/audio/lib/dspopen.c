/* dspopen.c
 * Program to open & close /dev/dsp device
 * Based on http://www.guerrilla.net/reference/dsp/prog_dsp.htm
 * Trilok Soni - x000soni@india.ti.com
 */

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "testlog.h"

char *progname;
testlog *logfile;

int waiter=0;
void usage(){
	printf(
		"Usage:\n"
			 "%s -h  -- Help you are looking at\n"
			 "%s read <device> -- To open device in read mode\n"
			 "%s write <device> -- To open device in write mode\n"
			 "%s readwrite <device> -- To open device in read/write mode\n", progname, progname, progname, progname
		);
}

int dsp_open_read(int argc, char* argv[]){

	int fd, ret;
	int errors = 0;
	int err_case = 0;

	if (argc < 1){
		errors++;
		case_fail(logfile, "please give proper device node");
		return errors;
	}
		
	if (waiter) {
		sleep(1);
	}
	fd = open(argv[0], O_RDONLY);
        if ((fd == -1) && (! err_case)) {
                errors++;
                case_fail_f(logfile, "%s - Open of %s file "
                        "with flag: O_RDONLY , failed with: %d\n",
                         __func__, argv[0], fd);
        }
        else if ((err_case) && (fd != -1)) {
                errors++;
                case_fail_f(logfile, "%s - Open of %s file "
                        "with flag: %s, should have returned a -1, but it "
                        "returned: %d.\n",__func__, argv[0],
                O_RDONLY, fd);
        } else
        case_pass(logfile, "open test in read mode");
	if (!waiter) {
		sleep(2);
	} else {
		sleep(1);
	}

        if ((ret = close(fd)) != 0) {
                test_fail_f(logfile, "%s - Problem "
                        "closing %s: %d.\n", __func__,argv[0], ret);
                errors++;
        }
	
	return errors;
}

int dsp_open_readwrite(int argc, char* argv[]){

	int fd;
	int errors = 0;
	int err_case = 0;
	int ret;

	if (argc < 1){
		errors ++;
		case_fail(logfile, "please give proper device node");
		return errors;
	}

	if (waiter) {
		sleep(1);
	}
	fd = open(argv[0], O_RDWR);
        if ((fd == -1) && (! err_case)) {
                errors++;
                case_fail_f(logfile, "%s - Open of %s file "
                        "with flag: O_RDWR , failed with: %d\n",
                         __func__, argv[0], fd);
        }
        else if ((err_case) && (fd != -1)) {
                errors++;
                case_fail_f(logfile, "%s - Open of %s file "
                        "with flag: %s, should have returned a -1, but it "
                        "returned: %d.\n",__func__, argv[0],
                O_RDWR, fd);
        } else
        case_pass(logfile, "open test in read write mode");
	if (!waiter) {
		sleep(2);
	} else {
		sleep(1);
	}

        if ((ret = close(fd)) != 0) {
                test_fail_f(logfile, "%s - Problem "
                        "closing %s: %d.\n", __func__,argv[0], ret);
                errors++;
        }

	return errors;
}
int dsp_open_write(int argc, char* argv[]){

	int fd;
	int errors = 0;
	int err_case = 0;
	int ret;

	if (waiter) {
		sleep(1);
	}
	if (argc < 1){
		errors++;
		case_fail(logfile, "please give proper device node\n");
		return errors;
	}

	fd = open(argv[0], O_WRONLY);
	if ((fd == -1) && (! err_case)) {
                errors++;
                case_fail_f(logfile, "%s - Open of %s file "
                        "with flag: O_WRONLY , failed with: %d[%d]\n",
                         __func__, argv[0], fd,waiter);
        }
        else if ((err_case) && (fd != -1)) {
                errors++;
                case_fail_f(logfile, "%s - Open of %s file "
                        "with flag: %s, should have returned a -1, but it "
                        "returned: %d[%d].\n",__func__, argv[0],
                O_WRONLY, fd,waiter);
        } else
        case_pass(logfile, "open test in write mode");
	if (!waiter) {
		sleep(2);
	} else {
		sleep(1);
	}

        if ((ret = close(fd)) != 0) {
                test_fail_f(logfile, "%s - Problem "
                        "closing %s: %d.\n", __func__,argv[0], ret);
                errors++;
        }

	return errors;
}
int main(int argc, char **argv)
{
  int errors = 0;
  struct option long_option [] = 
  {
	{"help", 0 , 0, 0},
	{0, 0, 0, 0},
  };
 
  progname = argv[0]; 

  logfile = test_create();
  addfilebyname(logfile, "dspopen_tests.dat", VERBOSE);
  test_init(logfile, "Starting user space tests for omap audio");
   
  while (1){
	int c;
	
	if ( (c = getopt_long (argc, argv, "hw", long_option, NULL )) < 0)
		break;
	
	switch (c) {
	case 'w':
		waiter=1;
	case 'h':
		usage ();
		break;
	defualt:
		errors ++;
		test_fail(logfile, "Switch statement in main: Should never have reached here. \n");
                return errors;
	}
  }

  if (argc - optind <= 0){
	usage (); 
	exit (1);
  }

  if (!strcmp (argv[optind], "read")) {
	case_start(logfile, "open device file in O_RDONLY");
	errors += dsp_open_read( argc - optind -1, argc - optind > 1 ? argv + optind + 1: NULL );	
  }
  else if (!strcmp (argv[optind], "write")) {
	case_start(logfile, "open device file in O_WRONLY");
	errors += dsp_open_write( argc - optind -1, argc - optind > 1 ? argv + optind + 1: NULL );	
  }
  else if (!strcmp (argv[optind], "readwrite")) {
	case_start(logfile, "open device file in O_RDWR");
	errors += dsp_open_readwrite( argc - optind -1, argc - optind > 1 ? argv + optind + 1: NULL );	
  }

  test_exit(logfile, "Completed user space tests fro omap audio");
  return errors;
}
