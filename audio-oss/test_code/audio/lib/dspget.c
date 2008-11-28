/* dspget.c
 * Program to get various information on /dev/dsp device.
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
#include <linux/soundcard.h>
#include <errno.h>

#include "test_log/testlog.h"

/* Borrowed from http://www.dtek.chalmers.se/groups/dvd/dist/oss_audio.c */
/* AFMT_AC3 is really IEC61937 / IEC60958, mpeg/ac3/dts over spdif */
#ifndef AFMT_AC3
#define AFMT_AC3        0x00000400      /* Dolby Digital AC3 */
#endif
#ifndef AFMT_S32_LE
#define AFMT_S32_LE     0x00001000  /* 32/24-bits, in 24bit use the msbs */ 
#endif
#ifndef AFMT_S32_BE
#define AFMT_S32_BE     0x00002000  /* 32/24-bits, in 24bit use the msbs */ 
#endif

testlog *logfile;
char *progname;

void usage(){
	printf(
		"Usage: %s -D </dev/dsp> [OPTIONS]\n\n"
		"[OPTIONS] \n"
		"  -h --help    - help\n" 
		"  -r --rate    - To get sampling rate\n"
		"  -b --bitsize	- To get sample size\n"
		"  -c --channels- To get no. of channels\n"
		"  -f --formats - To get supported audio formats\n"
		"  -a --all     - To get all information\n"
		, progname);
}

int dsp_list_rate(int fd, int *rate){

        int status;
	int errors = 0;

	if(ioctl(fd, SOUND_PCM_READ_RATE, rate)){
		case_fail_f(logfile, "%s - Unable to read sample rate: %s\n", strerror(errno));
		errors++;
		return errors;
	}
	case_pass(logfile, "Reading sample rate");

        return errors;
}

int dsp_list_channels(int fd, int *channels){

        int status;
	int errors;

        if(ioctl(fd, SOUND_PCM_READ_CHANNELS, channels)){
                case_fail_f(logfile, "%s - Unable to read no. of channels: %s\n", strerror(errno));
	        errors++;
		return errors;	
	}
	case_pass(logfile, "Reading no. of channels");
        return errors;
}


int dsp_list_bitsize(int fd, int *bitsize){

	int status;
	int errors;
	
	if(ioctl(fd, SOUND_PCM_READ_BITS, bitsize)){
	       case_fail_f(logfile, "%s - Unable to read sample size: %s\n", strerror(errno));
               errors++;
               return errors;
	}
	case_pass(logfile, "Reading sample size");
	
	return errors;
}

int dsp_get_fmts(int fd){

	char result[255];
	int status, formats, deffmt;
	int errors = 0;

#if SOUND_VERSION >= 301
	 printf("\nSupported Formats:\n");
	 deffmt = AFMT_QUERY;
	 if(ioctl(fd, SOUND_PCM_SETFMT, &deffmt)){
	       case_fail_f(logfile, "%s - Unable to set format: %s\n", strerror(errno));
               errors++;
               return errors;
	 }
	 
	 if(ioctl(fd, SOUND_PCM_GETFMTS, &formats)){
	       case_fail_f(logfile, "%s - Unable to get formats: %s\n", strerror(errno));
               errors++;
               return errors;
	 }

	 if (formats & AFMT_S32_LE) {
	   printf("  signed 32/24 little-endian");
	   (deffmt == AFMT_S32_LE) ? printf(" (default)\n") : printf("\n");
	 }
	 if (formats & AFMT_S32_BE) {
	   printf("  signed 32/24 Big-endian");
	   (deffmt == AFMT_S32_BE) ? printf(" (default)\n") : printf("\n");
	 }
	 if (formats & AFMT_MU_LAW) {
	   printf("  mu-law");
	   (deffmt == AFMT_MU_LAW) ? printf(" (default)\n") : printf("\n");
	 }
	 if (formats & AFMT_A_LAW) {
	   printf("  A-law");
	   (deffmt == AFMT_A_LAW) ? printf(" (default)\n") : printf("\n");
	 }
	 if (formats & AFMT_IMA_ADPCM) {
	   printf("  IMA ADPCM");
	   (deffmt == AFMT_IMA_ADPCM) ? printf(" (default)\n") : printf("\n");
	 }
	 if (formats & AFMT_U8) {
	   printf("  unsigned 8-bit");
	   (deffmt == AFMT_U8) ? printf(" (default)\n") : printf("\n");
	 }
	 if (formats & AFMT_S16_LE) {
	   printf("  signed 16-bit little-endian");
	   (deffmt == AFMT_S16_LE) ? printf(" (default)\n") : printf("\n");
	 }
	 if (formats & AFMT_S16_BE) {
	   printf("  signed 16-bit big-endian");
	   (deffmt == AFMT_S16_BE) ? printf(" (default)\n") : printf("\n");
	 }
	 if (formats & AFMT_S8) {
	   printf("  signed 8-bit");
	   (deffmt == AFMT_S8) ? printf(" (default)\n") : printf("\n");
	 }
	 if (formats & AFMT_U16_LE) {
	   printf("  unsigned 16-bit little-endian");
	   (deffmt == AFMT_U16_LE) ? printf(" (default)\n") : printf("\n");
	 }
	 if (formats & AFMT_U16_BE) {
	   printf("  unsigned 16-bit big-endian");
	   (deffmt == AFMT_U16_BE) ? printf(" (default)\n") : printf("\n");
	 }
	 if (formats & AFMT_MPEG) {
	   printf("  MPEG 2");
	   (deffmt == AFMT_MPEG) ? printf(" (default)\n") : printf("\n");
	 }
	 case_pass(logfile, "Reading supported formats");

	 switch(deffmt) {
		 case AFMT_S32_LE:
			 sprintf(result,"Format is |S32_LE|\n");break;
		 case AFMT_S16_LE:
			 sprintf(result,"Format is |S16_LE|\n");break;
		 default:
			 sprintf(result,"Format is |unknown|\n");break;
	 }
	case_start(logfile, "format");
	 case_pass(logfile, result);
#endif

	return errors;
}

int main(int argc, char **argv)
{
	char temp_string[255];
  int do_list_rate, do_list_bitsize, do_list_channels,
      do_list_all, do_list_formats;
  char *device_name = "/dev/dsp";
  int fd, ret = 0;
  int bitsize, channels;
  int rate;
  int errors, err_case;

  errors = err_case = 0;


  do_list_rate = do_list_bitsize = do_list_channels = do_list_all =0;
  do_list_formats = 0;

  struct option long_option [] = 
  {
	{"help", 0 , 0, 'h'},
	{"Device", 1, 0, 'D'},
	{"rate", 0, 0, 'r'},
	{"bitsize", 0, 0,'b'},
	{"channels", 0, 0, 'c'},
	{"formats", 0, 0, 'f'},
	{"all", 0, 0, 'a'},
	{0, 0, 0, 0},
  };


  bitsize = channels = 0;
 
  progname = argv[0]; 
	
  logfile = test_create();
  addfilebyname(logfile, "dspget_tests.dat", VERBOSE);
  test_init(logfile, "Starting user space tests for omap audio");
   
  while (1){
	int c;
	
	if ( (c = getopt_long (argc, argv, "hD:rbcaf", long_option, NULL )) < 0)
		break;
	
	switch (c) {

	case 'h':
		usage ();
		return 0;
	case 'D':
		device_name = optarg;
		break;
	case 'r':
		do_list_rate =1 ;
		break;
	case 'b':
		do_list_bitsize =1;
		break;
	case 'c':
		do_list_channels =1;
		break;
	case 'f':
		do_list_formats =1;
		break;
	case 'a':
		do_list_all=1;
		break;
	default:
		errors ++;
                test_fail(logfile, "Switch statement in main: Should never have reached here Try --help. \n");
                return errors;
		
	}
  }

  fd = open(device_name, O_RDWR);
  if ((fd == -1) && (! err_case)) {
 	 errors++;
         test_error_exit_f(logfile, "%s - Open of %s file "
      	 	"with flag: O_RDWR , failed with: %d\n",
	         __func__, device_name, fd);
  }
  else if ((err_case) && (fd != -1)) {
	 errors++;
         test_error_exit_f(logfile, "%s - Open of %s file "
                 "with flag: O_RDWR, should have returned a -1, but it "
                 "returned: %d.\n",__func__, device_name, fd);
  }	

  if (do_list_all ){
	errors += dsp_list_bitsize (fd, &bitsize);
	errors += dsp_list_channels (fd, &channels);
	errors += dsp_list_rate (fd, &rate) ;	
	errors += dsp_get_fmts(fd);
	printf("Sample size %d\n"
	       "No. of channels %d\n"
	       "Sampling rate %d\n", bitsize, channels, rate);
  }

  /*FIXME - Add proper return codes and messages here*/
  if (do_list_rate){
	case_start(logfile, "Reading sample rate");
	errors += dsp_list_rate (fd, &rate) ;	
	case_start(logfile, "sample rate");
	sprintf(temp_string, "Sample rate is |%d|\n", rate);
	case_pass(logfile, temp_string);
  }
  if (do_list_channels) {
	case_start(logfile, "Reading no. of channels");
	errors += dsp_list_channels (fd, &channels);
	case_start(logfile, "no. of channels");
	sprintf(temp_string, "No. of channels are |%d|\n", channels);
	case_pass(logfile, temp_string);
  }
  if (do_list_bitsize)  {
	case_start(logfile, "Reading sample size in bits");
	errors += dsp_list_bitsize (fd, &bitsize);
	case_start(logfile, "sample size in bits");
	sprintf(temp_string, "Sample size is |%d|\n", bitsize);
	case_pass(logfile, temp_string);
  }
  if (do_list_formats) {
	case_start(logfile, "Reading supported formates");
	errors +=dsp_get_fmts(fd);
  }

  if ((ret = close(fd)) != 0) {
	test_fail_f(logfile, "%s - Problem "
        "closing %s: %d.\n", __func__,argv[0], ret);
        errors++;
  }

  test_exit(logfile, "Completed user space tests for omap audio");

  return errors;
}
