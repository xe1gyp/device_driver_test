/*****************************************************************************
 * (C) Copyright Texas Instruments, 2003. All Rights Reserved.
 *
 * Name of the file: fbtest01
 *
 * Location of the file: PET/src/test/os/device_drivers/framebuffer/
 *
 * Brief description of the contents of the file:
 *
 * Test blank and unblank of framebuffer device
 * Test write to framebuffer
 *
 * Detailed description of the contents of the file:
 *
 * Author: Prathibha Tammana (Enhanced test based on an open source program)
 *
 * Created on: 9/22/03
 *
 * Change Log:
 *
 * 100103: Prathibha: Updated writing logic to write the word, test was only valid for 8bpp
 * 92503: Prathibha: Modified usage() function to actually display the usage
 * 	Changed the variable names to shorter names c style from c++ style - easier typing.
 * 92203: Prathibha: Added functions to display additional data in fixed and variable screen info structures
 * 	Added unblank option
 * 122303: Changed /dev/fb0 to /dev/fb/0 for devfs
 *
 *
 * Initial Version: Open Source
 *
 * fbtest
 *
 * $Id: fbtest.c,v 1.3 2003/12/23 23:31:03 testuser Exp $
 *
 * $Log: fbtest.c,v $
 * Revision 1.3  2003/12/23 23:31:03  testuser
 *
 * /dev/fb0 -> /dev/fb/0
 *
 * Revision 1.2  2003/10/03 20:27:02  a0868035
 *
 * Updated code to write to framebuffer (word vs byte manipulation)
 *
 * Revision 1.1.1.1  2003/09/26 14:20:58  a0868035
 *
 * - The first time I checked in my stuff, it got into the wrong place :(
 * - So consider this initial checkin. I would have lost the revision history :(
 * - prathibha
 *
 *
 * Revision 1.2  2003/09/25 23:25:10  a0868035
 *
 * Added a few more enhancements.
 * Prathibha - 9/25/03
 *
 * Revision 1.1.1.1  2003/09/25 20:42:17  a0868035
 * First import of framebuffer directory with design and tests
 * prathibha - 9/25/2003
 *
 *
 * Revision 1.1  2000/08/23 15:25:33  tim
 * Test utilities for Vrlinux frame buffer and flash driver.
 *
 * 
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fb.h>

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef enum { FALSE, TRUE } BOOL;
typedef unsigned char BYTE, *PBYTE;

struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;

WORD wDebug;
DWORD dwLength, dwFillChar;
BOOL fVersionFlag, fWriteFlag, fUnblankFlag, fBlankFlag;
char szVersion[] = "fbtest V1.0 2003/09/23";

void usage(void);

//************************************************************************
//
// Name:		DoArgumentParsing
//
// Description:	Parses command line arguments
//
// Returns:		TRUE if we like the arg's, FALSE if processing should stop
//
//************************************************************************
BOOL DoArgumentParsing(int argc, char *argv[])
{
	char *sThisArg;

	for ( ; --argc > 0; ) {
		sThisArg = *++argv;
		// Is this an option?
		if ((*sThisArg == '-') || (*sThisArg == '/')) {
			for (; *++sThisArg != 0; ) {
				switch (tolower(*sThisArg)) {

				case 'u':	// unblank
					fUnblankFlag = TRUE;
					break;

				case 'b':	// blank
					fBlankFlag = TRUE;
					break;

				case 'd':	// debug
					wDebug++;
					break;

				case 'h':	// help
					usage();
					return FALSE;
				case '?':
					usage();
					return FALSE;

				case 'l':	// length
					++sThisArg;
					if (*sThisArg == 0) {
						sThisArg = *++argv;
						argc--;
					}
					dwLength = strtoul(sThisArg, NULL, 0);
					if (wDebug)
						printf("Length set to 0x%x\n", dwLength);
					goto NextArg;

				case 'f':	// fill character
					++sThisArg;
					if (*sThisArg == 0) {
						sThisArg = *++argv;
						argc--;
					}
					dwFillChar = strtol(sThisArg, NULL, 0);
					if (wDebug > 0)
						printf("Fill set to 0x%02x\n", dwFillChar);
					goto NextArg;

				case 'v':	// version
					fVersionFlag = TRUE;
					return FALSE;

				case 'w':	// write
					fWriteFlag = TRUE;
					break; 

				default:
					printf("unknown option - %s\n", sThisArg);
					usage();
					return FALSE;
				}
			}
		}
	NextArg:
		;
	}
	return TRUE;
}

void usage(void)
{
	printf("fbtest [-h] [-u] [-b] [-l <size>] [-f <fill char>] [-d] [-w] [-v]\n");
	printf("default: print fixed and variable screen informaion sizes\n");
	printf("u: unblank\n");
	printf("b: blank\n");
	printf("h: help - this message \n");
	printf("l <size>: sets length of memory to write to\n");
	printf("	  Note: needs -w option to actually write\n");
	printf("f <fill char>: use the character supplied to writeinto the memory\n");
	printf("d: print debug messages\n");
	printf("w: write to framebuffer mapped memory\n");
	printf("v: print version of test\n");
	exit(0);
}

int main(int argc, char *argv[])
{
	int fbfd;
	int nRet = 0;
	PBYTE pBuffer;
	int i;
	unsigned char *p;

	fbfd = open("/dev/fb0", O_RDWR);
	if (fbfd == -1) {
		fprintf(stderr, "Couldn't open frame buffer - %s\n", 
			strerror(errno));
		exit(1);
	}

	nRet = ioctl( fbfd, FBIOGET_FSCREENINFO, 
		&finfo );
	if (nRet != 0) {
		fprintf(stderr, "Couldn't get Fixed info - %s\n", 
			strerror(errno));
		close(fbfd);
		exit(1);
	}

	nRet = ioctl( fbfd, FBIOGET_VSCREENINFO, 
		&vinfo );
	if (nRet != 0) {
		fprintf(stderr, "Couldn't get Var info - %s\n", 
			strerror(errno));
		close(fbfd);
		exit(1);
	}

	if (argc == 1) {
		printf("fb0 Fixed Info:\n"
				"   %s  @ 0x%lx, len=0x%x, line=0x%x (%d) bytes,\n",
			finfo.id,
			finfo.smem_start,
			finfo.smem_len,
			finfo.line_length,
			finfo.line_length);

		printf(
				"   Geometry - %u x %u, %u bpp%s\n",
			vinfo.xres, 
			vinfo.yres, 
			vinfo.bits_per_pixel,
			vinfo.grayscale ? ", greyscale" : "");

		printf("   Color - offset:length:msb_right \n");


		printf("   Greyscale %d\n",vinfo.grayscale);
		printf("   Nonstd %d\n",vinfo.nonstd);
		printf("   Red %d:%d:%d\n",vinfo.red.offset,vinfo.red.length, vinfo.red.msb_right);
	        printf("   Green %d:%d:%d\n",vinfo.green.offset,vinfo.green.length, vinfo.green.msb_right);
		printf("   Blue %d:%d:%d\n",vinfo.blue.offset,vinfo.blue.length, vinfo.blue.msb_right);
		printf("   Transparent %d:%d:%d\n",vinfo.transp.offset,vinfo.transp.length, vinfo.transp.msb_right);

		if ( finfo.visual == FB_VISUAL_TRUECOLOR) {
			printf(		"visual: FB_VISUAL_TRUECOLOR\n");
		} else {
			printf(		"visual: %d\n", finfo.visual);
		}


		if ( (vinfo.height != -1) || 
				(vinfo.width != -1)) {
			printf(
				"   %d mm high x %d mm wide\n", 
				vinfo.height, 
				vinfo.width);
		}
	}
	else if (!DoArgumentParsing(argc, argv)) {
		// make sure the command line makes sense
		if (fVersionFlag)
			printf("%s\n", szVersion);
		else
			usage();
		exit(1);
	}

	if (fUnblankFlag) {
		nRet = ioctl( fbfd, FBIOBLANK, 
			0 );
		if (nRet != 0) {
			fprintf(stderr, "Couldn't unblank - %s\n", 
				strerror(errno));
			close(fbfd);
			exit(1);
		}
	}

	if (fBlankFlag) {
		nRet = ioctl( fbfd, FBIOBLANK, 
			4 );
		printf("Return val of FBIOBLANK = %d\n", nRet);
		if (nRet != 0) {
			fprintf(stderr, "Couldn't blank - %s\n", 
				strerror(errno));
			close(fbfd);
			exit(1);
		}
	}

	if (fWriteFlag) {
		if (dwLength == 0) {
			dwLength = finfo.smem_len;
		}

		if ((pBuffer = malloc( dwLength )) == NULL) {
			fprintf(stderr, "Couldn't get buffer info - %s\n", 
				strerror(errno));
			close(fbfd);
			exit(1);
		}

		/*
		printf("val=0x%x, high=0x%x, low=0x%x\n", dwFillChar,
				(unsigned char) (dwFillChar & 0xff), (unsigned char) (dwFillChar >> 8) );
		*/
		p = pBuffer;
		for (i=0;i<dwLength;i=i+2) {
			*p++ = (unsigned char)(dwFillChar & 0xff);
			*p++ = (unsigned char) (dwFillChar >> 8);
		}
		//memset will only work if we are writing a byte. 16bpp needs a word	
		//To verify if correct bits are being written, cp /dev/fb/0 to a tmp file
		//and run hexdump on it.
		//memset( pBuffer, dwFillChar, dwLength );
		p = pBuffer;
		nRet = write( fbfd, pBuffer, dwLength );
		if (nRet != dwLength) {
			fprintf(stderr, "Couldn't write - %s\n", 
				strerror(nRet = errno));
		}
		else
			nRet = 0;
	}

	close(fbfd);
	return nRet;
}
