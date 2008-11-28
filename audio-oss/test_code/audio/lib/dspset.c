/* dspset.c
 * Program to set various information on /dev/dsp device.
 * Based on http://www.guerrilla.net/reference/dsp/prog_dsp.htm
 * Trilok Soni - x000soni@india.ti.com
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <linux/soundcard.h>
#include "test_log/testlog.h"

char *progname;
testlog *logfile;

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

/*very in-efficient style, as AFT_* is not aligned e.g. 0,1,2 with index*/
#define FORMAT(v) [AFMT_##v] = #v
#define FORMAT_LAST 1024 

static const char *dsp_set_formats [] = {
	FORMAT(MU_LAW),
	FORMAT(A_LAW),
	FORMAT(IMA_ADPCM),
	FORMAT(U8),
	FORMAT(S16_LE),
	FORMAT(S16_BE),
	FORMAT(S8),
	FORMAT(U16_LE),
	FORMAT(U16_BE),
	FORMAT(MPEG),
	FORMAT(AC3),
	FORMAT(S32_LE),
	FORMAT(S32_BE),
};

void usage(){
	printf(
		"Usage: %s -D </dev/dsp> [OPTIONS]\n"
                "[OPTIONS] \n"
		"  -h --help	   - help\n"
                "  -r --rate=#     - To set sampling rate\n"
                "  -b --bitsize=#  - To set sample size\n"
                "  -c --channels=# - To set no. of channels\n"
		"  -f --formats=#  - To set sample format\n"
                , progname
		);
	printf("Recognized sample formats are: \n"
		"MU_LAW, A_LAW, S16_LE, S16_BE, U8, S8"
		" U16_LE, U16_BE, MPEG, AC3, IMA_ADPCM, S32_LE, S32_BE\n"
		);	

}
#define AFMT_UNKNOWN 0x00000999
int dsp_format_value(const char *name){

	int format;
	printf(" name (%s)\n" ,name);
	for (format = 0; format <= FORMAT_LAST; format ++){
	/*FIXME- check presence of element in arrary */
		/*printf("%s", dsp_set_formats [format] );*/
		if( strcasecmp ( name, dsp_set_formats [format] ) > 0 ) {
			printf("name(%s)\n", name);
			return format;
		}
	}	
	return AFMT_UNKNOWN;
}

int dsp_set_format(int fd, int fmt){

	int status;	
	int lfmt, errors;

	errors = 0;

	lfmt = fmt;	
	if(ioctl(fd, SOUND_PCM_SETFMT, &lfmt)){
              case_fail_f(logfile, "%s - SOUND_PCM_SETFMT failed: %s\n", strerror(errno));
              errors++;
              return errors;
	}
	if (fmt != lfmt ){
	      case_fail_f(logfile, "%s - Unable to set sample format: %s\n", strerror(errno));
              errors++;
              return errors;
	}
	case_pass(logfile, "Setting sample format");

	return errors;
}

int dsp_set_rate(int fd, int rate){

        int status;
        int arg, errors;

        arg = rate;
        if(ioctl(fd, SOUND_PCM_WRITE_RATE, &arg)){
              case_fail_f(logfile, "%s - SOUND_PCM_WRITE_RATE failed: %s\n", strerror(errno));
              errors++;
              return errors;
	}
	printf("Returned rate(%d) and supplied rate(%d)\n", arg, rate);
        if (arg != rate){
              case_fail_f(logfile, "%s - Unable to set sample rate: %s\n", strerror(errno));
              errors++;
	      return errors;
        }
	case_pass(logfile, "Setting sample rate");

        return errors;
}

int dsp_set_channels(int fd, int channels){

        int status;
        int arg, errors;

	errors = 0;

        arg = channels;
        if(ioctl(fd, SOUND_PCM_WRITE_CHANNELS, &arg)){
              case_fail_f(logfile, "%s - SOUND_PCM_WRITE_CHANNELS failed: %s\n", strerror(errno));
              errors++;
              return errors;
	}
        if (arg != channels){
              case_fail_f(logfile, "%s - Unable to set no. of channels: %s\n", strerror(errno));
              errors++;
              return errors;
        }
	case_pass(logfile, "Setting no. of channels");

        return errors;
}


int dsp_set_bitsize(int fd, int bitsize){

	int status;
  	int arg, errors;
	
	errors = 0;
	
	arg = bitsize;	
	if(ioctl(fd, SOUND_PCM_WRITE_BITS, &arg)){
                case_fail_f(logfile, "%s - SOUND_PCM_WRITE_BITS failed: %s\n", strerror(errno));
                errors++;
                return errors;
	}
	if (arg != bitsize){
                case_fail_f(logfile, "%s - Unable to set sample size: %s\n", strerror(errno));
                errors++;
                return errors;
	}
	case_pass(logfile,"Setting sample size");
	
	return errors;
}

#define FORMAT2(v) AFMT_##v
int main(int argc, char **argv)
{
  int rate, bitsize, channels;
  char *device_name = "/dev/dsp";
  int fd, ret = 0;
  char*  temp;
  int do_set_format = 0;
  int fmt;
  int errors, err_case;

  errors = err_case = 0;

  struct option long_option [] = 
  {
	{"help", 0 , 0, 'h'},
	{"Device", 1, 0, 'D'},
	{"rate", 1, 0, 'r'},
	{"bitsize", 1, 0,'b'},
	{"channels", 1, 0, 'c'},
	{"formats", 1, 0, 'f'},
	{0, 0, 0, 0},
  };


  rate = bitsize = channels = 0;
 
  progname = argv[0]; 

  logfile = test_create();
  addfilebyname(logfile, "dspset_tests.dat", VERBOSE);
  test_init(logfile, "Starting user space tests for omap audio");
 
  while (1){
	int c;
	
	if ( (c = getopt_long (argc, argv, "hD:r:b:c:f:", long_option, NULL )) < 0)
		break;
	
	switch (c) {

	case 'h':
		usage ();
		return 0;
	case 'D':
		device_name = optarg;
		break;
	case 'r':
		rate = atoi(optarg);
		rate = rate < 4000 ? 4000: rate;
		rate = rate > 196000 ? 196000: rate;
		break;
	case 'b':
		bitsize = atoi(optarg);
		break;
	case 'c':
		channels = atoi(optarg);
		channels = channels < 1 ? 1: channels;
		channels = channels > 1024? 1024: channels;
		break;
	case 'f':
		temp = optarg;
		if ( strcasecmp (temp , "A_LAW") == 0  ){
			fmt = FORMAT2(A_LAW);
			do_set_format = 1;
		}

		else if ( strcasecmp (temp , "MU_LAW") == 0  ){
			fmt = FORMAT2(MU_LAW);
			do_set_format = 1;
		}

		else if ( strcasecmp (temp , "IMA_ADPCM") == 0  ){
			fmt = FORMAT2(IMA_ADPCM);
			do_set_format = 1;
		}

		else if ( strcasecmp (temp , "U8") == 0  ){
			fmt = FORMAT2(U8);
			do_set_format = 1;
		}

		else if ( strcasecmp (temp , "S16_LE") == 0  ){
			fmt = FORMAT2(S16_LE);
			do_set_format = 1;
		}

		else if ( strcasecmp (temp , "S16_BE") == 0  ){
			fmt = FORMAT2(S16_BE);
			do_set_format = 1;
		}

		else if ( strcasecmp (temp , "MPEG") == 0  ){
			fmt = FORMAT2(MPEG);
			do_set_format = 1;
		}
	
		else if ( strcasecmp (temp , "AC3") == 0  ){
			fmt = FORMAT2(AC3);
			do_set_format = 1;
		}
	
		else if ( strcasecmp (temp , "S32_LE") == 0  ){
			fmt = FORMAT2(S32_LE);
			do_set_format = 1;
		}
	
		else if ( strcasecmp (temp , "S32_BE") == 0  ){
			fmt = FORMAT2(S32_BE);
			do_set_format = 1;
		}
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
                 "returned: %d.\n",__func__, device_name,  fd);
  }

  /*FIXME - Add proper return codes and messages here*/
  if (rate){
	case_start(logfile, "Setting sample rate");
	errors += dsp_set_rate (fd,rate) ;	
  }
  if (channels){
	case_start(logfile, "Setting sample rate");
	errors += dsp_set_channels (fd, channels);
  }
  if (bitsize){
	case_start(logfile, "Setting sample size");
	errors += dsp_set_bitsize (fd, bitsize);	
  }
  if (do_set_format){
	case_start(logfile,"Setting audio sample format"); 
 	errors += dsp_set_format(fd, fmt);
  }	

  if ((ret = close(fd)) != 0) {
        test_fail_f(logfile, "%s - Problem "
        "closing %s: %d.\n", __func__,device_name, ret);
        errors++;
  }

  test_exit(logfile, "Completed user space tests for omap audio");

  return errors;
}
