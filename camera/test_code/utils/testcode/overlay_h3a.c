/* ================================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* ================================================================================ */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/videodev.h>

#include <time.h>

#define PREVIEW_ROTATION_NO		0
#define PREVIEW_ROTATION_0		1
#define PREVIEW_ROTATION_90		2
#define PREVIEW_ROTATION_180		3
#define PREVIEW_ROTATION_270		4
#define VIDIOC_S_OVERLAY_ROT		_IOW ('O', 1,  int)
#define VIDIOC_G_OVERLAY_ROT		_IOR ('O', 2,  int)
#define VIDIOC_ISP_DUMMY        _IOR ('O', 3,  int)
#define VIDIOC_ISP_2ACFG        _IOWR ('O', 6,  struct h3a_aewb_config)
#define VIDIOC_ISP_2AREQ        _IOWR ('O', 7,  struct h3a_aewb_data)
#define BYTES_PER_WINDOW	16

/* Flags for update field */
#define REQUEST_STATISTICS      (1 << 0)
#define SET_COLOR_GAINS         (1 << 1)
#define SET_DIGITAL_GAIN        (1 << 2)
#define SET_EXPOSURE            (1 << 3)
#define SET_ANALOG_GAIN         (1 << 4)

#ifndef u32
#define u32 unsigned long
#endif /* u32 */

#ifndef u16
#define u16 unsigned short
#endif /* u16 */

#ifndef u8
#define u8 unsigned char
#endif /* u8 */

//#define CAP_UTILS
#ifdef CAP_UTILS
#include "CapUtils.h"
#else
struct h3a_aewb_config{
	u16 saturation_limit;    
	u16 win_height;          // Range: 2 - 256
	u16 win_width;           // Range: 2 - 256
	u16 ver_win_count;       // vertical window count (Range: 1 - 128)
	u16 hor_win_count;       // horizontal window count (Range: 1 - 36)
	u16 ver_win_start;       // vertical window start position (Range: 0 -4095)
	u16 hor_win_start;       // horizontal window start position (Range: 0 - 4095)    
	u16 blk_ver_win_start;   // black line vertical window start position (Range: 0 -4095)
	u16 blk_win_height;      // black line height (Range: 2 - 256 even values only)
	u16 subsample_ver_inc;   // vertical distance between subsamples (Range: 2 - 32 even values only)
	u16 subsample_hor_inc;   // horizontal distance between subsamples (Range: 2 - 32 even values only)
	u8   alaw_enable;         // enable AEW ALAW flag
	u8   aewb_enable;         // AE AWB statistics generation enable flag    
};

struct h3a_aewb_data {
	void * h3a_aewb_statistics_buf;  // Pointer to AE AWB statistics buffer to be filled
	u32 shutter;                 // Shutter speed
	u16 gain;                    // Sensor Gain
	u32 shutter_cap;             // Shutter speed for capture
	u16 gain_cap;                // Sensor Gain for capture    

	u16 dgain;                   // White balance digital gain for preview module
	u16 wb_gain_b;               // White balance color gain blue
	u16 wb_gain_r;               // White balance color gain red
	u16 wb_gain_gb;              // White balance color gain green blue
	u16 wb_gain_gr;              // White balance color gain green red

	u16 frame_number;            // Requested frame
	u16 curr_frame;              // Current frame being processed
	u8 update;                   // Flags to update parameters
};
#endif

#ifdef CAP_UTILS
#define PIX_PER_WINDOW 25
h3a_aewb_paxel_data_t h3a_stats[H3A_AEWB_MAX_WIN_NUM];
h3a_aewb_paxel_data_t h3a_avg[1];
#endif

u16 speed_test_results[40][2];

static void usage(void)
{
	printf("overlay [0|1|2] [rotno|rot0|rot90|rot180|rot270]\n");
	printf("\tTo stop/start camera preview. 0: stop, 1: start on V1, 2: start on V2\n");
	printf("\tPreview can optionally rotate the image and will last 5 mintues\n");
}

int main (int argc, char *argv[])
{
	int ret, fd, enable = 1, rotation = -1, oldrotation;
    	int i, j = 0;
	unsigned int num_windows = 0, num_color_windows = 0;
	unsigned int buff_size = 0;
	struct h3a_aewb_config aewb_config_user;
	struct h3a_aewb_data aewb_data_user;
	u16 *buff_preview = NULL;
	u16 *buff_char = NULL;
	unsigned int buff_prev_size = 0;
	int data8, data2, window, unsat_cnt;
	int input, new_gain = 0;
	int frame_number;

	if ((argc > 1) && (!strcmp(argv[1], "?"))) {
		usage();
		return 0;
	}


	if (argc >= 2) {
		if (!strcmp(argv[1], "rot0"))
			rotation = 1;
		else if (!strcmp(argv[1], "rot90"))
			rotation = 2;
		else if (!strcmp(argv[1], "rot180"))
			rotation = 3;
		else if (!strcmp(argv[1], "rot270"))
			rotation = 4;
		else if (!strcmp(argv[1], "rotno"))
			rotation = 0;
		else if ((!strcmp(argv[1], "0")) ||
			 (!strcmp(argv[1], "1")) ||
			 (!strcmp(argv[1], "2")))
			enable = atoi(argv[1]);
		else {
			usage();
			return 0;
		}
	}
	if (!rotation && argc >= 3) {
		if (!strcmp(argv[2], "rot0"))
			rotation = 1;
		else if (!strcmp(argv[2], "rot90"))
			rotation = 2;
		else if (!strcmp(argv[2], "rot180"))
			rotation = 3;
		else if (!strcmp(argv[2], "rot270"))
			rotation = 4;
		else if (!strcmp(argv[2], "rotno"))
			rotation = 0;
	}

	if (enable != 0 && enable != 1 && enable != 2) {
		usage();
		return 0;
	}



	if ((fd = open_cam_device(O_RDWR)) <= 0) {
		printf ("Could not open the cam device\n");
		return -1;
	}

	
	ret = ioctl (fd, VIDIOC_G_OVERLAY_ROT, &oldrotation);
	if (ret == 0) {
	      	if (oldrotation)
			printf("Old Rotation: %d\n",(oldrotation-1)*90);
		else
			printf("Old Rotation: No\n");
	}

	if (enable & rotation != -1) { 
		ret = ioctl (fd, VIDIOC_S_OVERLAY_ROT, &rotation);
		if (ret == 0) {
			ret = ioctl (fd, VIDIOC_G_OVERLAY_ROT, &rotation);
			if (ret != 0)
				perror("VIDIOC_G_OVERLAY_ROT");
			else {
			      	if (rotation)
					printf("Rotation %d ...\n",(rotation-1)*90);
				else
					printf("No rotation ...\n");
			}
		}
	} 
	
	ret = ioctl (fd, VIDIOC_OVERLAY, &enable);
	if (ret != 0) {
		perror("VIDIOC_OVERLAY");
		printf("Can't enable overlay...\n");
		return -1;
	}

	if (enable > 0) {
		printf("Previewing on Video%d\n", enable);
		sleep(5);

/* ************************* H3A TEST **************************** */
		/* H3A params */
		aewb_config_user.saturation_limit = 0x1FF;
		aewb_config_user.win_height = 10;
		aewb_config_user.win_width = 10;
		aewb_config_user.ver_win_count = 3;
		aewb_config_user.hor_win_count = 3;
		aewb_config_user.ver_win_start = 10;
		aewb_config_user.hor_win_start = 10;
		aewb_config_user.blk_ver_win_start = 30;
		/* blk_win_height MUST NOT be the same height as win_height*/
		aewb_config_user.blk_win_height = 8;	
		aewb_config_user.subsample_ver_inc = 2;
		aewb_config_user.subsample_hor_inc = 2;
		aewb_config_user.alaw_enable = 1;
		aewb_config_user.aewb_enable = 1;

		/* set h3a params */
		ret = ioctl(fd, VIDIOC_ISP_2ACFG, &aewb_config_user);
		if (ret < 0) {
			printf("Error: %d, ", ret);
			perror ("VIDIOC_ISP_2ACFG");
			return ret;
		}
		
		num_windows = ((aewb_config_user.ver_win_count
				* aewb_config_user.hor_win_count)
				+ aewb_config_user.hor_win_count);				;
        	num_color_windows = (aewb_config_user.ver_win_count
				* aewb_config_user.hor_win_count);
		buff_size = ((num_windows + (num_windows / 8) + 
			     ((num_windows % 8) ? 1 : 0)) * BYTES_PER_WINDOW);
		buff_prev_size = (buff_size / 2);

//		printf("Buffer size = %d bytes\n", buff_size);

		/* Again, user needs a buffer */
//		buff_preview = malloc(buff_size);
//		buff_char = buff_preview;
//		if (buff_preview == NULL) {
//			printf("Can't allocate user buffer! \n");
//			return -ENOMEM;
//		}
		aewb_data_user.h3a_aewb_statistics_buf = NULL; 
//		aewb_data_user.h3a_aewb_statistics_buf = (void *)buff_preview; 

		/* Digital gain in U10Q8 format */
		aewb_data_user.dgain = 0x100;	/* Gain = 1.000 */
		/* Particular gains in U8Q5 format */
		aewb_data_user.wb_gain_b = 0x94;
		aewb_data_user.wb_gain_r = 0x68;
		aewb_data_user.wb_gain_gb = 0x5C;
		aewb_data_user.wb_gain_gr = 0x5C;
		
		/* Shutter & gain for preview */
		/* Exposure time between 26 and 65000 microseconds */
		aewb_data_user.shutter = 20000;
		/* Gain between 0x08 and 0x7F */
		aewb_data_user.gain = 0x40;
		
		aewb_data_user.update = (SET_COLOR_GAINS | SET_DIGITAL_GAIN
					 | SET_EXPOSURE | SET_ANALOG_GAIN);
		aewb_data_user.frame_number = 8; //dummy

		printf("Setting first parameters \n");
		ret = ioctl(fd, VIDIOC_ISP_2AREQ, &aewb_data_user);
		if (ret < 0) {
			perror("VIDIOC_ISP_2AREQ");
			return ret;
		}
		aewb_data_user.frame_number = aewb_data_user.curr_frame + 3;
//		goto waiting;

request:	
		frame_number = aewb_data_user.frame_number;
		// request stats 
		printf("Requesting stats for frame %d, try %d\n", frame_number, j);
		ret = ioctl(fd, VIDIOC_ISP_2AREQ, &aewb_data_user);
		if (ret < 0) {
			perror("VIDIOC_ISP_2AREQ");
			return ret;
		}

		if (aewb_data_user.h3a_aewb_statistics_buf == NULL) {
			printf("NULL buffer, current frame is  %d.\n",
				aewb_data_user.curr_frame);
			aewb_data_user.frame_number =
						aewb_data_user.curr_frame + 10;
			aewb_data_user.update = REQUEST_STATISTICS;
//			aewb_data_user.h3a_aewb_statistics_buf = (void *)buff_char;
//			buff_preview = buff_char;
			goto request;
		} else {
		// Display stats
//			printf("Received buffer = %p \n",
//				aewb_data_user.h3a_aewb_statistics_buf);
			buff_preview = (u16 *)aewb_data_user.h3a_aewb_statistics_buf;
			printf("H3A AE/AWB: buffer to display = %d data pointer = %p\n",
            			buff_prev_size, aewb_data_user.h3a_aewb_statistics_buf);
			printf("num_windows = %d\n", num_windows);
//			printf("ver_windows = %d\n", aewb_config_user.ver_win_count);
//			printf("hor_windows = %d\n", aewb_config_user.hor_win_count);
//			printf("plus one row of black windows\n");
			
			unsat_cnt = 0;
			for (i = 0; i < (buff_prev_size); i++) {
				data8 = (i + 1 ) % 8;
				data2 = (i + 1 ) % 2;
				window = (i + 1) / 8;
				printf("%05d ", buff_preview[i]);
				if (0 == data8) {
					if (((window > 1) && (0 == (window % 9)))
						|| (window ==
						((num_windows + (num_windows / 8) + 
				     		((num_windows % 8) ? 1 : 0))))) {
						printf("   Unsaturated block count\n");
						unsat_cnt++;
					}
					else {
						printf("    Window %5d\n",
							(window - 1) - unsat_cnt);
					}
				}
				if (0 == data2)
					printf("\n");
			}
		}

		sleep(1);

		j++;
		if (j < 2) {
			aewb_data_user.frame_number += 100;
			aewb_data_user.update = REQUEST_STATISTICS;
//			aewb_data_user.h3a_aewb_statistics_buf = (void *)buff_char;
//			buff_preview = buff_char;
			goto request;
		}
waiting:			
//		printf("Before the while...\n");
		while (1) {
			input = getchar();
			if (input == 'y') {
				aewb_data_user.dgain = aewb_data_user.dgain + 0x10;
				printf("Set new dgain: %u\n", aewb_data_user.dgain);
				new_gain = 1;
                aewb_data_user.update = (SET_DIGITAL_GAIN);
			}
			else if (input == 'h') {
				aewb_data_user.dgain = aewb_data_user.dgain - 0x10;
				printf("Set new dgain: %u\n", aewb_data_user.dgain);
				new_gain = 1;
                aewb_data_user.update = (SET_DIGITAL_GAIN);
			}
			else if (input == 'u') {
				aewb_data_user.wb_gain_b = aewb_data_user.wb_gain_b + 0x10;
				printf("Set new bgain: %u\n", aewb_data_user.wb_gain_b);
				new_gain = 1;
                aewb_data_user.update = (SET_COLOR_GAINS);
			}
			else if (input == 'j') {
				aewb_data_user.wb_gain_b = aewb_data_user.wb_gain_b - 0x10;
				printf("Set new bgain: %u\n", aewb_data_user.wb_gain_b);
				new_gain = 1;
                aewb_data_user.update = (SET_COLOR_GAINS);
			}
			else if (input == 'i') {
				aewb_data_user.wb_gain_r = aewb_data_user.wb_gain_r + 0x10;
				printf("Set new rgain: %u\n", aewb_data_user.wb_gain_r);
				new_gain = 1;
                aewb_data_user.update = (SET_COLOR_GAINS);
			}
			else if (input == 'k') {
				aewb_data_user.wb_gain_r = aewb_data_user.wb_gain_r - 0x10;
				printf("Set new rgain: %u\n", aewb_data_user.wb_gain_r);
				new_gain = 1;
                aewb_data_user.update = (SET_COLOR_GAINS);
			}
			else if (input == 'o') {
				aewb_data_user.wb_gain_gb = aewb_data_user.wb_gain_gb + 0x10;
				printf("Set new gb gain: %u\n", aewb_data_user.wb_gain_gb);
				new_gain = 1;
                aewb_data_user.update = (SET_COLOR_GAINS);
			}
			else if (input == 'l') {
				aewb_data_user.wb_gain_gb = aewb_data_user.wb_gain_gb - 0x10;
				printf("Set new gb gain: %u\n", aewb_data_user.wb_gain_gb);
				new_gain = 1;
                aewb_data_user.update = (SET_COLOR_GAINS);
			}
			else if (input == 'p') {
				aewb_data_user.wb_gain_gr = aewb_data_user.wb_gain_gr + 0x10;
				printf("Set new gr gain: %u\n", aewb_data_user.wb_gain_gr);
				new_gain = 1;
                aewb_data_user.update = (SET_COLOR_GAINS);
			}
			else if (input == ';') {
				aewb_data_user.wb_gain_gr = aewb_data_user.wb_gain_gr - 0x10;
				printf("Set new gr gain: %u\n", aewb_data_user.wb_gain_gr);
				new_gain = 1;
                aewb_data_user.update = (SET_COLOR_GAINS);
			}
            else if (input == 't') {
				aewb_data_user.gain = aewb_data_user.gain + 0x04;
				printf("Set new again: %u\n", aewb_data_user.gain);
				new_gain = 1;
                aewb_data_user.update = (SET_ANALOG_GAIN);
			}
			else if (input == 'g') {
				aewb_data_user.gain = aewb_data_user.gain - 0x04;
				printf("Set new again: %u\n", aewb_data_user.gain);
				new_gain = 1;
                aewb_data_user.update = (SET_ANALOG_GAIN);
			}
            else if (input == 'r') {
				aewb_data_user.shutter = aewb_data_user.shutter + 2000;
				printf("Set new shutter: %u\n", aewb_data_user.shutter);
				new_gain = 1;
                aewb_data_user.update = (SET_EXPOSURE);
			}
			else if (input == 'f') {
				aewb_data_user.shutter = aewb_data_user.shutter - 2000;
				printf("Set new shutter: %u\n", aewb_data_user.shutter);
				new_gain = 1;
                aewb_data_user.update = (SET_EXPOSURE);
			}
            else if (input == 's') { /* speed test */
                int speed_counter, sframe_counter, scurrent_frame, stest_start;
                struct timespec stime;
                stime.tv_sec = 0;
                stime.tv_nsec = 1000000; /* 1 millisec sleep */
                aewb_data_user.update = 0;
                ret = ioctl(fd, VIDIOC_ISP_2AREQ, &aewb_data_user);
				if (ret < 0) {
					perror("VIDIOC_ISP_2AREQ");
					return ret;
				}
                
                aewb_data_user.frame_number = aewb_data_user.curr_frame;
                aewb_data_user.update = (REQUEST_STATISTICS);
                ret = ioctl(fd, VIDIOC_ISP_2AREQ, &aewb_data_user);
				if (ret < 0) {
					perror("VIDIOC_ISP_2AREQ");
					return ret;
				}
                stest_start = scurrent_frame = aewb_data_user.curr_frame;
                
                printf ("Speed test: start:%d\n",scurrent_frame);
                for (sframe_counter = 0; sframe_counter < 40; sframe_counter++) {
                    speed_counter = 0;
                    while (scurrent_frame == aewb_data_user.curr_frame) {
                        speed_counter++;
                        aewb_data_user.update = 0;
                        ret = ioctl(fd, VIDIOC_ISP_2AREQ, &aewb_data_user);
                        nanosleep(&stime, NULL);
                        printf("STX: %d %d curr_frame:%d\n",scurrent_frame, speed_counter, aewb_data_user.curr_frame);
                    }
                    aewb_data_user.update = (REQUEST_STATISTICS);
                    aewb_data_user.frame_number = scurrent_frame;
                    printf("ST:requesting frame %d\n",aewb_data_user.frame_number);
                    ret = ioctl(fd, VIDIOC_ISP_2AREQ, &aewb_data_user);
                    scurrent_frame = aewb_data_user.curr_frame;
                    speed_test_results[sframe_counter][0] = (u16) scurrent_frame;
                    speed_test_results[sframe_counter][1] = (u16) speed_counter;
                    printf("ST:%d %d ptr:%p\n",scurrent_frame, speed_counter, aewb_data_user.h3a_aewb_statistics_buf);
                }
                
                #if 0
                for (sframe_counter = 0; sframe_counter < 40; sframe_counter++) {
                    printf ("Speed test: start:%d\n",stest_start);
                    printf("ST:%d %d\n",speed_test_results[sframe_counter][0], speed_test_results[sframe_counter][1]);
                }
                #endif
			}
            else if (input == '1') {
                aewb_data_user.update = 0;
                ret = ioctl(fd, VIDIOC_ISP_2AREQ, &aewb_data_user);
				if (ret < 0) {
					perror("VIDIOC_ISP_2AREQ");
					return ret;
				}
                aewb_data_user.frame_number = aewb_data_user.curr_frame;
                aewb_data_user.update = (REQUEST_STATISTICS);
                
                printf("Obtaining stats frame:%d\n", aewb_data_user.frame_number);
                ret = ioctl(fd, VIDIOC_ISP_2AREQ, &aewb_data_user);
				if (ret < 0) {
					perror("VIDIOC_ISP_2AREQ");
					return ret;
				}
                
                #ifdef CAP_UTILS
				h3a_aewb_get_data(h3a_stats, aewb_data_user.h3a_aewb_statistics_buf, num_color_windows);
                h3a__aewb_get_color_average(h3a_stats, num_color_windows, PIX_PER_WINDOW, h3a_avg);
                printf("Averages: red:%u green:%u blue:%u\n", h3a_avg[0].red, h3a_avg[0].green, h3a_avg[0].blue);
                #endif
                
                {
                    // Display stats 
            		buff_preview = (u16 *)aewb_data_user.h3a_aewb_statistics_buf;
            		printf("buffer pointer = %p\n",
            			buff_preview);
 //           		printf("num_windows = %d\n", num_windows);
 //           		printf("ver_windows = %d\n", aewb_config_user.ver_win_count);
 //           		printf("hor_windows = %d\n", aewb_config_user.hor_win_count);
 //           		printf("plus one row of black windows\n");	

                    unsat_cnt = 0;
            		for (i = 0; i < (buff_prev_size); i++) {
            			data8 = (i + 1 ) % 8;
            			data2 = (i + 1 ) % 2;
            			window = (i + 1) / 8;
            			printf("%05d ", buff_preview[i]);
            			if (0 == data8) {
            				if (((window > 1) && (0 == (window % 9)))
            					|| (window ==
            					((num_windows + (num_windows / 8) + 
            			     		((num_windows % 8) ? 1 : 0))))) {
            					printf("   Unsaturated block count\n");
            					unsat_cnt++;
            				}
            				else {
            					printf("    Window %5d\n",
            						(window - 1) - unsat_cnt);
            				}
            			}
            			if (0 == data2)
            				printf("\n");
            		}
                }
                
                
                
			}
			else if (input == 'q') break;
			
			if (new_gain == 1) {
				ret = ioctl(fd, VIDIOC_ISP_2AREQ, &aewb_data_user);
				if (ret < 0) {
					perror("VIDIOC_ISP_2AREQ");
					return ret;
				}
				new_gain = 0;
			}
		}
		// Display stats 
/*		buff_preview = (u16 *)buff_start;
		printf("H3A AE/AWB: buffer to display = %d data \n",
			buff_prev_size);
		printf("num_windows = %d\n", num_windows);
		printf("ver_windows = %d\n", aewb_config_user.ver_win_count);
		printf("hor_windows = %d\n", aewb_config_user.hor_win_count);
		printf("plus one row of black windows\n");	

		for (i = 0; i < (buff_prev_size); i++) {
			data8 = (i + 1 ) % 8;
			data2 = (i + 1 ) % 2;
			window = (i + 1) / 8;
			printf("%05d ", buff_preview[i]);
			if (0 == data8) {
				if (((window > 1) && (0 == (window % 9)))
					|| (window ==
					((num_windows + (num_windows / 8) + 
			     		((num_windows % 8) ? 1 : 0))))) {
					printf("   Unsaturated block count\n");
					unsat_cnt++;
				}
				else {
					printf("    Window %5d\n",
						(window - 1) - unsat_cnt);
				}
			}
			if (0 == data2)
				printf("\n");
		}
*/
		sleep(5);
		
		// disable 
		aewb_config_user.aewb_enable = 0;
		ret = ioctl(fd, VIDIOC_ISP_2ACFG, &aewb_config_user);
		if (ret < 0) {
			printf("Error: %d, ", ret);
			perror ("VIDIOC_ISP_2ACFG disabling");
			return ret;
		}
		

/* ******************** ENDS H3A TEST ******************** */	
		sleep(300);
		
		enable = 0;
		ret = ioctl (fd, VIDIOC_OVERLAY, &enable);
		if (ret != 0)	{
			perror("VIDIOC_OVERLAY");
			return -1;
		}
	}

	printf("Preview stopped!\n");
	close(fd);
}




