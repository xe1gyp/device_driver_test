/*********************************************************************************
Name of the file: fb_user_tests.c

Location: PET/src/test/device_drivers/framebuffer/fb_user_tests.c

Description:
	User space tests for framebuffer device driver.
	Refer to the design for the complete description of test cases

	Features Tested:
		fb_open
		fb_write
		fb_mmap
		fb_ioctl
			FBIOGET_FSCREENINFO
			FBIOGET_VSCREENINFO
			FBIOPUT_VSCREENINFO
			FBIOPUTCMAP
			FBIOGETCMAP
Created On: 09/28/03
Change Log:
09/28/03 - Prathibha - Initial Version
10/03/03 - Added several additional test cases. Note that there is no emphasis on checking the errno, just the return code. Several portions of linux code is not 100% POSIX compliant on return codes.

Author: Prathibha Tammana (prathibha@ti.com)
**********************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include "fb_user_tests.h"

#ifdef TESTLOG_LIBRARY_AVAILABLE
#include "libs/log/testlog.h"
testlog *logfile;
#endif


/*
 * call_test_ioctl: The function that tests the various arguments to ioctl
 *
 * Arguments:
 * framebuffer device file descriptor
 * type of ioctl test
 *
 * Return Value:
 * zero if success, number of failures if not
 *
 *
 *
 */

int call_test_ioctl(int fd, int tst_type)
{

	int errors=0;
	int ret;
	struct fb_fix_screeninfo finfo;
	struct fb_var_screeninfo vinfo;
	struct fb_cmap cmap;
	unsigned short int red[4095], green[4095], blue[4095];
	unsigned short int transp[4095];

	switch (tst_type) {
		case TST_BAD_IOCTL_CMD:
			ret = ioctl(fd, FBIOBADIOCTL, 0);
			if (ret != -1) {
				case_fail(logfile, "ioctl with command did not fail\n");
				errors++;
			}
			case_pass(logfile, "test for invalid ioctl cmd");
			break;

		case TST_SCREENINFO:
			if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo)) {
				case_fail_f(logfile, "call_test_ioctl - FBIOGET_FSCREENINFO: %s\n", strerror(errno));
				errors++;
			}

			if (! (ioctl(fd, FBIOGET_FSCREENINFO, NULL))) {
				case_fail_f(logfile, "call_test_ioctl - FBIOGET_FSCREENINFO with null: %s\n",strerror(errno));
				errors++;
			}

			if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo)) {
				case_fail_f(logfile, "call_test_ioctl - FBIOGET_VSCREENINFO\n:%s",strerror(errno));
				errors++;
			}

			if ( !(ioctl(fd, FBIOGET_VSCREENINFO, NULL)) ) {
				case_fail_f(logfile, "call_test_ioctl - FBIOGET_VSCREENINFO with null: %s\n",strerror(errno));
				errors++;
			}
			if (errors == 0) case_pass(logfile, "test for screeninfo ioctl");
			break;
		case TST_CMAP:
			cmap.start = 0;
			cmap.len = 26;
			cmap.red = red;
			cmap.green = green;
			cmap.blue = blue;
			cmap.transp = transp;

			printf("CMAP str: Start= %d Len=%d Red= 0x%x Blue= 0x%x Green= 0x%x \n",cmap.start,cmap.len,cmap.red,cmap.blue,cmap.green);
			if (ioctl(fd, FBIOGETCMAP, &cmap)){
				case_fail_f(logfile, "call_test_ioctl - FBIOGETCMAP: %s\n",strerror(errno));
				errors++;
			}
			if ( !(ioctl(fd, FBIOGETCMAP, NULL)) ) {
				case_fail_f(logfile, "call_test_ioctl - FBIOGETCMAP with null: %s\n", strerror(errno));
				errors++;
			}
			printf("CMAP str: Start= %d Len=%d Red= 0x%x Blue= 0x%x Green= 0x%x \n",cmap.start,cmap.len,cmap.red,cmap.blue,cmap.green);
			if (errors == 0) case_pass(logfile, "test for colormap ioctl");
			break;
		default:
			errors++;
			case_fail(logfile, "Switch statement in call_test_ioctl: Should never have reached here. \n");

	} /* end of switch */

	return errors;

}	/* end of call_test_ioctl */

/* call_test_write : The function that mmaps framebuffer memory and
 * test write. This function also changes the colormap
 * and virtual screen info
 *
 * Arguments: frame buffer file descriptor
 * flag indicating the  test type
 *
 * Return Value
 * zero upon pass and a non-zero integer upon failure
 */

int call_test_write(int fd, int flag)
{
	int errors = 0;
	int ret,i,width,height,row,column,color;
	unsigned int length;
	unsigned char *data;
	struct fb_cmap cmap, sav_cmap;
	struct fb_fix_screeninfo finfo;
	struct fb_var_screeninfo vinfo;
	unsigned short int red[4095], green[4095], blue[4095];
	unsigned short int r=2, g=2, b=2;
	unsigned short int transp[4095];
	int colorcount=0;
	unsigned short int colorarray[4] = {0x00,0xff,0x0f,0xf0};
	int return_val;

	switch (flag) {
		case TST_CMAP:
			/* 16 bpp does not set palette.
			 * We will not be allowed to change the color map
			 */
			cmap.start = 0;
			cmap.len = 16;
			cmap.red = red;
			cmap.green = green;
			cmap.blue = blue;
			cmap.transp = transp;
			if (ioctl(fd, FBIOGETCMAP, &cmap)) {
				case_fail_f(logfile, "call_test_write - Could not get current color map:%s\n",strerror(errno));
				errors++;
				break;
			}
			test_info_f(logfile, "call_test_write - current color map\n"
				"%d	%d	0x%x	0x%x	0x%x	0x%x\n",
				cmap.start, cmap.len, *cmap.red,
				*cmap.green, *cmap.blue, *cmap.transp);
			r = r << 12;
			g = g << 8;
			b = b << 4;
			test_info_f(logfile, "colormap for write test	r:0x%x	g:0x%x	b:0x%x\n", r, g, b);
			cmap.red[0] = r;
			cmap.green[0] = g;
			cmap.blue[0] = b;
			cmap.transp[0] = 0;
			if (ioctl(fd, FBIOPUTCMAP, &cmap)) {
				case_fail_f(logfile, "call_test_write - Could not update color map\n",strerror(errno));
				errors++;
				break;
			}
			test_info_f(logfile, "new cmap to be set %d\n", cmap.len);

			for(i=0; i<cmap.len; i++) {
				test_info_f(logfile, "r 0x%x, g 0x%x, b 0x%x\n", cmap.red[i], cmap.green[i], cmap.blue[i]);
			}

			if(ioctl(fd, FBIOGETCMAP, &cmap)) {
				case_fail_f(logfile, "call_test_write - Could not get updated color map:%s\n",strerror(errno));
				errors++;
				break;
			}
			test_info_f(logfile, "recd cmap\n %d", cmap.len);
			for(i=0; i<cmap.len; i++) {
				test_info_f(logfile, "r 0x%x, g 0x%x, b 0x%x\n", cmap.red[i], cmap.green[i], cmap.blue[i]);
			}

			test_info_f(logfile, "call_test_write - changed color map\n"
				"%d	%d	0x%x	0x%x	0x%x	0x%x\n",
				cmap.start, cmap.len, *cmap.red,
				*cmap.green, *cmap.blue, *cmap.transp);
			//initial color map is all zeroes, so this test is valid
			if ( (cmap.red[0] == r) || (cmap.green[0] ==g) || (cmap.blue[0] == b)) {
				case_fail_f(logfile, "call_test_write - Update to color map was allowed unexpectedly:%s\n",strerror(errno));
				errors++;
				break;
			}
			case_pass(logfile, "write test for colormap");
			break;
		case TST_SCREENINFO:
			if( ioctl(fd, FBIOGET_FSCREENINFO, &finfo)) {
					test_fail(logfile, "unable to read fixed screen info");
					errors++;
					break;
			}
			if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo)) {
				case_fail_f(logfile, "call_test_write - Could not get var screen info for write:%s\n",strerror(errno));
				errors++;
				break;
			}
			vinfo.xres=150;
			vinfo.yres=329;
			//if ( !(ioctl(fd, FBIOPUT_VSCREENINFO, &vinfo)) ) {
			return_val = ioctl(fd, FBIOPUT_VSCREENINFO, &vinfo);
			printf("RETURN_VAL = %d \n",return_val);
			if ( !return_val) {
				case_fail_f(logfile, "call_test_write -  Could not put var screen info for write: %s\n", strerror(errno));
				errors++;
				break;
			}
			case_pass(logfile, "write test for screen info");
			break;
		case TST_DATA:
			if( ioctl(fd, FBIOGET_FSCREENINFO, &finfo)) {
					test_fail(logfile, "unable to read fixed screen info");
					errors++;
					break;
			}
			if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo)) {
				case_fail_f(logfile, "call_test_write - Could not get var screen info for write: %s\n",strerror(errno));
				errors++;
				break;
			}
			width =vinfo.xres;
			height = vinfo.yres;
			data = mmap(0,width*height*2,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
			if (data <= 0) {
				case_fail_f(logfile, "call_test_write - Unable to mmap: %s\n",strerror(errno));
				printf("MMAP returned error\n");
				errors++;
				break;
			} else {
				// Write color bars of different colors to the screen
				// change color for every 20 rows.
				// color array has the color bytes to use
				// Note that since we do not have the logic included
				// to write words, we are using only bytes for color
				for (row=0;row<2*height;row++) {
					if (!(row % 20)) {
					if (colorcount == 4 ) colorcount = 0;
					else colorcount++;
					}
					for(column=0;column<width;column++) {
						data[column + row * width]= colorarray[colorcount];
					}
				}
				case_pass(logfile, "write test for framebuffer");
			}
			break;
		default:
			errors++;
			test_fail(logfile, "Switch statement in call_test_write: Should never have reached here. \n");
			return errors;
	} /* end of switch */

	return errors;

} /* end of call_test_write */

/*
 * call_test_open: The function that tests the open() function of
 * the omap_audio component of the module under test.
 *
 * Arguments:
 * None.
 *
 * Return value:
 * Zero upon pass and a non-zero integer upon failure.
 *
 */

int call_test_open(int flag)
{
	int errors =0;
	int fd;
	char flag_name[BUF_LEN];
	int err_case = 0;
	int ret;

	switch (flag) {
		case O_WRONLY:
			strcpy(flag_name, "O_WRONLY");
			break;
		case O_RDONLY:
			strcpy(flag_name, "O_RDONLY");
			break;
		case O_RDWR:
			strcpy(flag_name, "O_RDWR");
			break;
		case O_CREAT:
			strcpy(flag_name, "O_CREAT");
			break;
		default:
			errors++;
			test_fail(logfile, "Switch statement in call_test_open: Should never have reached here. \n");
			return errors;


	}

        /*
         * Open the device file corresponding to the test driver
         */
        fd = open(DEV_FILE, flag);
	if ((fd == -1) && (! err_case)) {
		errors++;
		case_fail_f(logfile, "call_test_open - Open of %s file "
			"with flag: %s, failed with: %d\n",
			DEV_FILE, flag_name, fd);
	}
	else if ((err_case) && (fd != -1)) {
		errors++;
		case_fail_f(logfile, "call_test_open - Open of %s file "
			"with flag: %s, should have returned a -1, but it "
			"returned: %d.\n", DEV_FILE,
		flag_name, fd);
	} else
	case_pass(logfile, "open test");

	if ((ret = close(fd)) != 0) {
		test_fail_f(logfile, "call_test_open - Problem "
			"closing %s: %d.\n", DEV_FILE, ret);
		errors++;
	}

	return errors;
}

/*
 * main: The main function calling the tests.
 *
 * Arguments:
 * None.
 * Return value:
 * Zero upon pass and a positive integer (incremented for each failure) upon
 * failure.
 *
 */
int main(int argc, char **argv)
{
	int ret = 0;
	int errors = 0;
	int fd;

	/*
	 * Make sure that it is root that is invoking this
	 */
	if (geteuid() != 0) {
		test_error_exit_f(logfile, "You have to be root, in order to be able to open the device file: %s \n", DEV_FILE );
		return NOT_ROOT_FAIL;
	}

#ifdef TESTLOG_LIBRARY_AVAILABLE
	logfile = test_create();
	addfilebyname(logfile, "fb_user_tests.dat", VERBOSE);
#endif
	test_init(logfile, "Starting User Space tests for Framebuffer");


	case_start(logfile, "open device file O_RDONLY");
	errors += call_test_open(O_RDONLY);

	case_start(logfile, "open device file O_WRONLY");
	errors += call_test_open(O_WRONLY);

	case_start(logfile, "open device file O_RDWR");
	errors += call_test_open(O_RDWR);

	case_start(logfile, "open device file O_CREAT");
	errors += call_test_open(O_CREAT);


	/*
	 * Open the device file corresponding to the test driver
	 */
	if ((fd = open(DEV_FILE, O_RDWR)) == -1) {
		test_error_exit_f(logfile, "Open of %s file failed with: %d.\n",
		             DEV_FILE, fd);
		return DEV_OPEN_FAIL;
	}

	case_start(logfile, "test for invalid ioctl command");
	errors += call_test_ioctl(fd, TST_BAD_IOCTL_CMD);

	case_start(logfile, "test for screeninfo ioctls");
	errors += call_test_ioctl(fd, TST_SCREENINFO);

	case_start(logfile, "test for colormap ioctls");
	errors += call_test_ioctl(fd, TST_CMAP);

	case_start(logfile, "write test for colormap");
	errors += call_test_write(fd, TST_CMAP);

	case_start(logfile, "write test for screeninfo");
	errors += call_test_write(fd, TST_SCREENINFO);

	case_start(logfile, "write test  for framebuffer");
	errors += call_test_write(fd, TST_DATA);

	if ((ret = close(fd)) != 0) {
		test_fail_f(logfile, "Problem closing %s: %d.\n", DEV_FILE, ret);
		errors++;
	}

	if (errors) {
		test_info_f(logfile, "Exiting with errors value: %d.\n", errors);
	}

	test_exit(logfile, "Completed User Space tests for Framebuffer");
	return errors;

}				/* end of main */
