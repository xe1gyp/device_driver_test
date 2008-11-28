#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <error.h>
#include <sys/ioctl.h>

#include "testlog.h"

#define TEN_BYTES 10
#define TWELVE_BYTES 12
#define FOURTY_BYTES 40
#define NINETY_BYTES 90
#define ONE_KB  1024 
#define ONE_MB 	1048576
#define TWO_MB  2097152
#define TEN_MB  10485760

#define PASS 0
#define FAIL -1

#define GOOD 1
#define BAD  0	

#define SET_PASSWORD 		1
#define CHANGE_PASSWORD 	2
#define LOCK_CARD 		3
#define UNLOCK_CARD 		4
#define RESET_PASSWORD 		5
#define FORCE_ERASE 		6
#define CARD_INFO		7

#define MMC 1

#define DEVICE_FILE    "/dev/mmcblk0p1"
#define MOUNT_PATH     "/mnt/mmc1"

int flag=PASS;

int card_presence(testlog *log);
int function(int case_no, char *first_arg, char *second_arg,testlog *log);
int store_retrieve(char *path, int size, testlog *log);



int main(int argc,char *argv[])
{

	int ret;
	int case_no=0;					 /* case number */
	char *first_arg=(char *)malloc(NINETY_BYTES);	 /* stores the contents of argv[1] */
        char *second_arg=(char *)malloc(NINETY_BYTES);	 /* stores the contents of argv[2] */ 
	char *line=(char *)malloc(NINETY_BYTES);	 /* line read from the mmc_input file is stored */
	char *value=(char *)malloc(FOURTY_BYTES);	 /* stores the case id from the mmc_input file */
	
	FILE *fp_input ;				 /* file descriptor for mmc_input file */

	testlog *log = test_create();			 /* to log error messages */	

        if( addfilebyname (log,"/mmctest_result",VERBOSE) < 0 )	/* Create "error_msg" file to log messages */
        {
                fprintf(stderr,"could not open logfiles");
                exit(-1);
        }

        test_init(log,"Starting MMC test");			

	if(argc==1)					/* if no case id  is given read data from the mmc_input file */
	{
		
		//if((fp_input = fopen("mmc_input", "r" ))==NULL)
		if((fp_input = fopen("/mmctest_input", "r" ))==NULL)
	        {
                	printf("Could not open the mmctest_input file \n");
        	        exit(-1);
        	}
		
	        while(fgets(line,NINETY_BYTES,fp_input)!=NULL)
        	{
                	sscanf(line,"%s %s %s",value,first_arg,second_arg );
			
			case_no=atoi(value);

			switch( case_no )
			{
				
				case 1 :
				case 4 :
				case 5 :
				case 9 :
				case 15:				
				case 16:
				case 22:
				case 23:
				case 26:
				case 27:
				case 28:
				case 29:
                                case 31:                                
				case 32:
                                case 33:
                                case 35:
				case 36:							      
					memset(second_arg,0,FOURTY_BYTES);
					break;	

				case 24:				
				case 25:
				case 34:
					memset(first_arg,0,FOURTY_BYTES);
                                        memset(second_arg,0,FOURTY_BYTES);
                                        break;


			} /* switch */
				
				if(case_no < 29)
		                if(case_no!=22)
		                if(case_no!=23)
			        if(case_no!=25)
			                 card_presence(log);

				ret = function( case_no,first_arg, second_arg, log);                	

	        }/* end of while loop */
	
		fclose(fp_input);

	}/* count=1 */	


	if(argc==2)				/* if no input arguments to the test case */
	{
		case_no=atoi(argv[1]);

	        if(case_no < 29)
		if(case_no!=22)
		if(case_no!=23)
		if(case_no!=25)
                 card_presence(log);

		memset(first_arg,0,FOURTY_BYTES);
		memset(second_arg,0,FOURTY_BYTES);

                ret = function( case_no,first_arg, second_arg, log);	
	}				
		
	if(argc==3)				/* if one input argument to the test case */
	{
		case_no=atoi(argv[1]);

                if(case_no < 29)
                if(case_no!=22)
                if(case_no!=23)
                if(case_no!=25)
                 card_presence(log);
		
		strcpy(first_arg,argv[2]);	
		memset(second_arg,0,FOURTY_BYTES);	     

		ret = function( case_no,first_arg, second_arg, log);
	}

	if(argc==4)				/* if two input arguments test case  */
	{
		case_no=atoi(argv[1]);

                if(case_no < 29)
                if(case_no!=25)
                if(case_no!=22)
                if(case_no!=23)
    	            card_presence(log);
	
                strcpy(first_arg,argv[2]);
               	strcpy(second_arg,argv[3]);
	
		ret = function( case_no,first_arg, second_arg, log);
	}

	
	if(argc>4)				/* if more than two input arguments to the test case */
	{
		 test_error(log,"Specified the command line arguments should not be more than FOUR");
		 ret = 1;
	}	

	test_exit(log,"MMC test exit"); 

	free(first_arg);
        free(second_arg);
	free(line);
	free(value);

        return ret;

}/* main*/


int card_presence(testlog *log)
{
	int fd , card_type ;

	FILE *fp1 ;

	/* variables to hold file system information */
        char filesystem[FOURTY_BYTES],noblocks[FOURTY_BYTES] ,disk_usage[FOURTY_BYTES];
        char disk_available[FOURTY_BYTES],percent_use[FOURTY_BYTES],mounted_on[FOURTY_BYTES] ;

	char *string=(char *)malloc(NINETY_BYTES);      /* to store string */
        char *line=(char *)malloc(NINETY_BYTES);        /* Store the line read from the file */
		

	if( (fd=open(DEVICE_FILE,O_RDWR) ) == -1)               /* open the device file */
        {
		test_warn(log,"Unexpected Error: Unable to open device file");
       		test_exit(log,"MMC test exit");
		exit(-1);
        }

        card_type=ioctl(fd,CARD_INFO,0);                /* request for card present */
	card_type = MMC;
	if( card_type!= MMC)
	{
		test_error(log,"No card found in the slot");
		test_exit(log,"MMC test exit");
		exit(-1);	
	}

	close(fd);

        unlink("mount.dat");

        strcpy(string,"df -hmk ");
        strcat(string,DEVICE_FILE);
        strcat(string," > mount.dat");

        if( system(string) == -1)     /* Redirect the information about the file system to mount.dat */
        {
              test_warn(log,"Unexpected Error: could not execute system command");
              test_info(log,"Failed to detect MMC card");
              test_exit(log,"MMC test exit");
              fclose(fp1);
              exit(-1);
        }

       if(( fp1 = fopen("mount.dat","r" )) == NULL )      /* Open the redirected file */
       {
              test_warn(log,"Unexpected Error: could not open file");
              test_exit(log,"MMC test exit");
              exit(-1);
       }

       if(fgets(line,NINETY_BYTES,fp1)!=NULL)               /* Get the first line from redirected file */
       {
              if(fgets(line,NINETY_BYTES,fp1)!=NULL)       /* Get second line from redirected file */
              {
   	             sscanf(line,"%s %s %s %s %s %s",filesystem,noblocks ,disk_usage,disk_available,percent_use,mounted_on);
                    /*Verify the type , disk space usage and device is mounted on */
	
                    if((strcmp(mounted_on,MOUNT_PATH)!=0))
                    {
                            test_info(log,"Failed to detect MMC card");
                            unlink("mount.dat");
                            fclose(fp1);
			    test_exit(log,"MMC test exit");
                            exit(-1);
                    }

                } /* if fget line 2 */				
		
		else
		{				
			test_info(log,"MMC Card is not mounted");
			fclose(fp1);
			unlink("mount.dat");
                        test_exit(log,"MMC test exit");
                        exit(-1); 
		}
		
        } /* if fget line 1 */

        fclose(fp1);
	unlink("mount.dat");

	free(string);
	free(line);

	return 0;
}


int store_retrieve(char *path,int size,testlog *log)
{
	int fd, i, no_times ;
	
        /* variables to hold file listings */
        char permissions[FOURTY_BYTES],type[FOURTY_BYTES],user[FOURTY_BYTES],cuser[FOURTY_BYTES],bytes[FOURTY_BYTES];
        char month[FOURTY_BYTES],date[FOURTY_BYTES],time[FOURTY_BYTES],filep[FOURTY_BYTES];
	char buffer[ONE_KB];

	char *file_list=(char *)malloc(NINETY_BYTES);      /* to store string */
        char *line=(char *)malloc(NINETY_BYTES);        /* Store the line read from the file */            

	FILE *fp;

	for(i=0;i<ONE_KB;i++)
	{
		buffer[i]='1';	
	}
	
	no_times=size/ONE_KB ;	

        if((fd = open(path,O_CREAT|O_EXCL|O_WRONLY, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH))==-1)
        {
	       	 test_warn(log,"File does not exist");
                 case_fail(log,"Failed  to store and retrieve data to a file");                               
                 return -1;
        }

        for(i=0;i<no_times;i++)                       /* write characters to the file */
        {
                 if(write(fd,&buffer,ONE_KB)==-1)
                 {
       		          test_warn(log,"Could not write into the file");
                          case_fail(log,"Failed  to store and retrieve data to a file");
                          close(fd);
                          return -1;
                 }
        } /* end of for loop */

        close(fd);                                      /* close the file */

        if( system("sync") == -1)                       /* flush filesystem buffers */
        {
                 case_fail(log,"Failed to write into the file");
                 close(fd);
                 test_warn(log,"Unexpected error: unable to execute system command");
                 test_exit(log,"MMC test exit");
                 exit(-1);
        }

        strcpy(file_list,"ls -l ");
        strcat(file_list,path);
        strcat(file_list," > compare1.dat");

        if(system(file_list)==-1)                       /* Redirect the file time and date */
        {
                 test_warn(log,"Unexpected Error: Could not read file attributes");
                 case_fail(log,"Failed  to store and retrieve data to a file");
                 test_exit(log,"MMC test exit");
                 exit(-1);
        }

        if(( fp = fopen("compare1.dat","r" )) == NULL )   /* Open the redirected file */
        {
                 test_warn(log,"Unexpected Error: could not open the file");
                 case_fail(log,"Failed  to store and retrieve data to a file");
                 close(fd);
                 return -1;
        }

        while(fgets(line,NINETY_BYTES,fp)!=NULL)       /* get a line from touch.dat */
        {
                 sscanf(line,"%s %s %s %s %s %s %s %s %s",permissions,type,user,cuser,bytes,month,date,time,filep);

        } /* end of while loop */

        if(atoi(bytes)!=size)
        {
                 test_error(log,"File is not as expected");
                 case_fail(log,"Failed  to store and retrieve data to a file");
                 unlink("compare1.dat");
		 fclose(fp);
                 return -1;
         }
		
	unlink("compare1.dat");
	free(file_list);
	free(line);
	fclose(fp);
        close(fd);
	return 0;

} /* store_retrieve */

	
int function(int case_no, char *first_arg, char *second_arg,testlog *log)
{

	int fd,fdr,fdw;					/* file descriptors */
	//int card_type;					/* Type of card */
	int i;						/* loop variable */
        unsigned int noBytes;        			/* No of bytes */
        char writeChar='1',readChar;			/* write character and read character */
        char *string=(char *)malloc(NINETY_BYTES);	/* to store string */
        char delim[]="/";				/* Store the Delimiter */
        char *temp;					/* Temporary pointer */ 
        char *path=(char *)malloc(NINETY_BYTES);	/* Hold the path */
        char *copy_path=(char *)malloc(NINETY_BYTES);	/* Copy of the path */
        char *temp_path=(char *)malloc(NINETY_BYTES);	/* Temporary Copy of the path */
        char *filename=(char *)malloc(NINETY_BYTES);	/* File name */
        char *hold;					/* Pointer to hold */
	char *compare=(char *)malloc(NINETY_BYTES);	/* To compare the strings */
	int cfp;					
	char read_buf;      				/* Store the read buffer */	
	char *line=(char *)malloc(NINETY_BYTES);	/* Store the line read from the file */	

	int i_month;					/* month of the file */	
        char *arg_year=(char *)malloc(NINETY_BYTES);	/* Store the year of the file */
        char *arg_month=(char *)malloc(NINETY_BYTES);	/* Store the month of the file */
        char *arg_day=(char *)malloc(NINETY_BYTES);	/* Store the week day of the file */
        char *file_list=(char *)malloc(NINETY_BYTES);	/* Hold the file listings */
	char *directory_name=(char *)malloc(NINETY_BYTES);	/* Store directory name */
	long int no_times,card_size;
	int loop ;
	 


	/* variables to hold file listings */
	char permissions[FOURTY_BYTES],type[FOURTY_BYTES],user[FOURTY_BYTES],cuser[FOURTY_BYTES],bytes[FOURTY_BYTES];
	char month[FOURTY_BYTES],week[FOURTY_BYTES],date[FOURTY_BYTES],time[FOURTY_BYTES],year[FOURTY_BYTES];
	char filep[FOURTY_BYTES];
	char permissions_ch[FOURTY_BYTES];

	/* variables to hold file system information */
	char filesystem[FOURTY_BYTES],noblocks[FOURTY_BYTES] ,disk_usage[FOURTY_BYTES];
	char disk_available[FOURTY_BYTES],percent_use[FOURTY_BYTES],mounted_on[FOURTY_BYTES] ;

	FILE *fp1,*fp2;					/* File descriptors */

        DIR *dp, *dpr  ;				/* Directory descriptor */
        struct dirent *dirp;				/* Pointer of type dirent */
	int check_path = GOOD;				/* Flag for to check the path */


        if(strstr(first_arg,"/")!=NULL)			/* check for the path */
        {
                 if(strstr(first_arg,MOUNT_PATH)==NULL)
                 {
			   check_path = BAD;	
                 }
                 else
                 {
                           strcpy(path,first_arg);
			   check_path = GOOD;		
                 }
        }

	else
        {
                  strcpy(path,MOUNT_PATH);
                  strcat(path,"/");
                  strcat(path,first_arg);
		  check_path = GOOD;		
        }

        flag=PASS;
	switch ( case_no )
        {

	       /*************************** Create a file *******************************************/
		case 1:
                        case_start(log,"Running test 1: Create a file in MMC card");


                        if(!check_path)			/* verify the path is correct */
                        {
				test_warn(log,"Specified path is incorrect");
                                case_fail(log,"Failed to create the file");
                                flag=FAIL;
                                break;
                        }


                        if ( (fd = open(path, O_CREAT|O_EXCL , S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH ) ) == -1 )
                        {
                           	printf("path=%s",path);;
				case_fail(log,"Failed to create the file");
                                flag=FAIL;
                                break;
                        }

                        case_pass(log,"File creation successful");
                        flag=PASS;
                        close(fd);

                        break;

                /************************** Write into a file ********************************************/

                case 2:

                        case_start(log,"Running test 2: Write into a file in MMC card");
		
			flag=PASS;

                        if(!check_path)						 /* verify the path is correct */
                        {
				test_warn(log,"Specified path is incorrect");
                                case_fail(log,"Failed to write into the file");
				flag=FAIL;
                                break;
			}

                        if ( ( fd = open(path,O_WRONLY) ) == -1 )		/* open the file for writing */
                        {

                                test_warn(log,"File does not exist");
                                case_fail(log,"Failed to write into the file");
				flag=FAIL;
                                break;
                        }

                        noBytes=atoi(second_arg);

                        for(i=0;i < noBytes ;i++)			/* write characters to the file */
                        {
                                if(write(fd,&writeChar,1)==-1)
                                {
                                        test_warn(log,"Could not write into the file");
                                        case_fail(log,"Failed to write into the file");
					flag=FAIL;
                                        close(fd);
                                        break;
                                }
                        } /* end of for loop */

			close(fd);					/* close the file */

			if( system("sync") == -1)			/* flush filesystem buffers */
			{
				case_fail(log,"Failed to write into the file");
                                flag=FAIL;
                                close(fd);
				test_warn(log,"Unexpected error: unable to execute system command");
                                test_exit(log,"MMC test exit");
                                exit(-1);
			}

		        if ( ( fd = open(path,O_RDONLY) ) == -1 )	/* reopen the written file */ 
                        {
                                test_warn(log,"Cannot open the written file");
                                case_fail(log,"Failed to write into the file");
				flag=FAIL;
                                break;
                        }

                        for(i=0;i < noBytes ;i++)			/* read the characters of the file */
                        {
                                if(read(fd,&readChar,1)==-1)
                                {

                                        test_warn(log,"Could not read back the file written");
                                        case_fail(log,"Failed to write into the file");
					flag=FAIL;
					close(fd);
                                        break;

                                }

                                if(readChar!=writeChar)			/* Verify the characters of the file */
                                {

                                        test_warn(log,"Data Read did not match with the Data Written");
                                        case_fail(log,"Failed to write into the file");
					flag=FAIL;
					close(fd);
                                        break;
                                }


                        } /* end of for loop */

			if(flag!=FAIL)
			{
                        	case_pass(log,"File write successful");
	                        flag=PASS;
			}

			close(fd);
                        
                        break;

		 /************************** Read the file **********************************************/
		case 3:		

			flag=PASS;

			case_start(log,"Running test 3: Read a file in MMC card");
					
                        if(!check_path)		 /* verify the path is correct */
                        {
				test_warn(log,"Specified path is incorrect");
                        	case_fail(log,"Failed to read the file");
				flag=FAIL;
                       		break;
			}

                        if( (fd = open(path,O_RDONLY ) ) == -1 )	/* open the file for reading */
                        {

				test_error(log,"File does not exist");
				case_fail(log,"Failed to read the file");
				flag=FAIL;
				break;

                        }

                        noBytes=atoi(second_arg);

                        for(i=0;i<noBytes;i++)		/* Read the characters of the file */
                        {
                                if(read(fd,&readChar,1)!=1)
                                {

					test_error(log,"Reached End of file before reading specified number of bytes");
	                                case_fail(log,"Failed to read the file");
	                                flag=FAIL;
					close(fd);
        	                        break;				

                                }

		                if(readChar!=writeChar)	  /* Verify the characters of the file */
                                {
					
                                        test_warn(log,"Data Read did not match with the Data Written");
					case_fail(log,"Failed to read the file");
	                                flag=FAIL;
                                        close(fd);
                                        break;
					
                                }
                        } /* end of for loop */
			
			if(flag!=FAIL)
			{
				case_pass(log,"File read successful");	
	                        flag=PASS;
			}
	
                        close(fd);
                        break;

		
		 /***************************** Modify the file ****************************************/
                case 4:

			case_start(log,"Running test 4: Modify the file in MMC card");

			flag=PASS;

                        if(!check_path)		/* Verify the path is correct */
                        {
				test_warn(log,"Specified path is incorrect");
                                case_fail(log,"Failed to modfiy the file");
				flag=FAIL;
                                break;
			}

                        if( (fd =open(path,O_RDWR)) == -1)	/* Open the file to be modified */
                        {
				test_error(log,"Specified File does not exist");
                                case_fail(log,"Failed to modfiy the file");
				flag=FAIL;
                                break;

			
                        }

                        writeChar='2';

                        for( i=0 ; i<TEN_BYTES ; i++ )		/* Write ten bytes of data to the file */
                        {
                                if(write(fd,&writeChar,1)==-1)
                                {
					test_error(log,"Unable to write to specified file");
					case_fail(log,"Failed to modfiy the file");
					flag=FAIL;
					close(fd);
					break;
                                }
                        } /* end of for loop */

			close(fd);

			if( system("sync") == -1)                       /* flush filesystem buffers */
                        {
                                case_fail(log,"Failed to modify the file");
                                flag=FAIL;
                                close(fd);
                                test_warn(log,"Unexpected error: unable to execute system command");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }

			if( (fd =open(path,O_RDWR)) == -1)	/* Reopen the modified file */
                        {
                                test_error(log," Unable to open the modified file");
                                case_fail(log,"Failed to modfiy the file");
                                flag=FAIL;
                                break;
                        }
	
                        for( i=0 ; i<TEN_BYTES ; i++ )		/* read ten bytes of data */ 
                        {
                                if(read(fd,&readChar,1)!=1)
                                {
        	                          test_error(log,"Unable to read the modified file for verification");
                	                  case_fail(log,"Failed to modfiy the file");
					  flag=FAIL;
                        	          close(fd);
                                	  break;
                                }

                                if(writeChar!=readChar)		/* Verify the written data */
                                {
                                        test_warn(log,"Data Read did not match with the Data Written");
					case_fail(log,"Failed to modfiy the file");
					flag=FAIL;
                                        close(fd);
                                        break;
                                }

                        } /*end of for loop */


			if(flag!=FAIL)
			{
				case_pass(log,"File modification successful");
	                        flag=PASS;
			}
			
                        close(fd);
                        break;

		/************************** Append the file *******************************************/

		case 5:
			case_start(log,"Running test 5: Append the file in MMC card");

                        flag=PASS;

                        if(!check_path)					/* Verify the path is correct */
                        {
                                case_fail(log,"Failed to append the file");
                                flag=FAIL;
                                break;
                        }

                        if( (fd =open(path,O_WRONLY|O_APPEND)) == -1)		/* Open the file to be modified */
                        {
                                test_error(log,"Specified File does not exist");
                                case_fail(log,"Failed to append the file");
                                flag=FAIL;
                                break;
                        }

                        writeChar='3';

                        for( i=0 ; i<TEN_BYTES ; i++ )				/* Write into the file */
                        {
                                if(write(fd,&writeChar,1)==-1)
                                {
                                        test_error(log,"Unable to write to specified file");
                                        case_fail(log,"Failed to append the file");
                                        flag=FAIL;
                                        close(fd);
                                        break;
                                }
                        } /* end of for loop */

                        close(fd);						/* Close the file */

			if( system("sync") == -1)                       /* flush filesystem buffers */
                        {
                                case_fail(log,"Failed to append  the file");
                                flag=FAIL;
                                close(fd);
                                test_warn(log,"Unexpected error: unable to execute system command");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }
			
                        if( (fd =open(path,O_RDWR)) == -1)			/* Reopen the modified file */
                        {
                                test_error(log," Unable to open the modified file");
                                case_fail(log,"Failed to append the file");
                                flag=FAIL;
                                break;


                        }

                        if(lseek(fd,-TEN_BYTES,SEEK_END)==-1)			/* Seek back ten bytes of data */
                        {

                                test_error(log,"Error seeking the modified file for verification");
                                case_fail(log,"Failed to append the file");
                                flag=FAIL;
                                close(fd);
                                break;

                        }

                        for( i=0 ; i<TEN_BYTES ; i++ )
                        {
                                if(read(fd,&readChar,1)!=1)			 /* read ten bytes of data */
                                {
                                          test_error(log,"Unable to read the modified file for verification");
                                          case_fail(log,"Failed to append the file");
                                          flag=FAIL;
                                          close(fd);
                                          break;

                                }

                                if(writeChar!=readChar)				 /* Verify the written data */
                                {
                                        test_warn(log,"Data Read did not match with the Data Written");
                                        case_fail(log,"Failed to append the file");
                                        flag=FAIL;
                                        close(fd);
                                        break;

                                }

                        }/* end of for loop */


                        if(flag!=FAIL)
                        {
                                case_pass(log,"Appending data to file is successful");
	                        flag=PASS;
                        }

                        close(fd);
                        break;

			
					
                /************************** Rename the file *******************************************/
                case 6:

			case_start(log,"Running test 6: Rename the file in MMC card"); 


                        if(!check_path)					/* Verify the path is correct */
                        {
				 test_warn(log,"Specified path is incorrect");
                                 case_fail(log,"Failed to rename the file");                                        
                                 flag=FAIL;
                                 break;
			}

                        strcpy(string,MOUNT_PATH);
                        strcat(string,"/");
                        strcat(string,second_arg);

                        if(rename(path,string)==-1)			/* Rename the file */
                        {
				test_error(log,"Specified File does not exist");
				case_fail(log,"Failed to rename the file");				
                                flag=FAIL;
				break;
                        }

			case_pass(log,"File renamed successfully");
                        flag=PASS;
                        break;

	
		/************************** Storing and retrieving different size of data ***************/
	        case 7:
			case_start(log,"Running test 7: Storing and retrieving different size of data in MMC card");
			
			 /* open the file for writing */			

			strcpy(path,MOUNT_PATH);
			strcat(path,"/f1");				

			if(store_retrieve(path,ONE_MB,log)==-1)
			{				
                                flag=FAIL;
				break;
			}

			unlink(path);					/* Remove the copied file */

                        strcpy(path,MOUNT_PATH);
                        strcat(path,"/f2");	

			if(store_retrieve(path,TWO_MB,log)==-1)
                        {
                                flag=FAIL;
                                break;
                        }					                        

			unlink(path);					/* Remove the copied file */
			
                        strcpy(path,MOUNT_PATH);
                        strcat(path,"/f3");

			if(store_retrieve(path,TEN_MB,log)==-1)
                        {
                                flag=FAIL;
                                break;
                        }

			unlink(path);					/* Remove the copied file */
			
			case_pass(log,"File store and retrieve operation successful");
                        flag=PASS;

                        break;			
		
		
                /******************** Change the file access permissions ****************************/
                case 8:
			case_start(log,"Running test 8: Change the file access permissions in MMC card"); 

                        if(!check_path)		/* Verify the path is correct */
                        {
				test_warn(log,"Specified path is incorrect");
                               	case_fail(log,"Failed to change the file access permissions");
                                flag=FAIL;
				break;
			}

			unlink("chmod_file1.dat");                               					
	
			strcpy(string,"ls -l ");
			strcat(string,path);
			strcat(string," > chmod_file1.dat ");

			if(system(string)==-1)	/* Redirect the file access permissions to a new file */
			{
				test_warn(log,"Unexpected error: unable to execute system command");
                               	case_fail(log,"Failed to change the file access permissions");
				test_exit(log,"MMC test exit");
				exit(-1);
			}		
			
                        if(( fp1 = fopen("chmod_file1.dat","r" )) == NULL )	/* Open the redirected file */
                        {
                                test_warn(log,"Unexpected error: unable to open the file");
                               	case_fail(log,"Failed to change the file access permissions");
                                flag=FAIL;
                                break;
                        }

                        while(fgets(line,NINETY_BYTES,fp1)!=NULL)	/* get a line form the redirected file */
                        {
                                sscanf(line,"%s ",permissions);

                        }

			unlink("chmod_file2.dat");                              	

			strcpy(temp_path,"chmod ");
			strcat(temp_path,second_arg);
			strcat(temp_path," ");
			strcat(temp_path,path);
		
			if(system(temp_path)==-1)	/* Change the permissions of the file */
			{
				test_warn(log,"Unexpected error: unable to execute system command");
                               	case_fail(log,"Failed to change the file access permissions");
				unlink("chmod_file1.dat");	                        
				fclose(fp1);
				test_exit(log,"MMC test exit");
                                exit(-1);
			}

			strcpy(copy_path,"ls -l ");	
			strcat(copy_path,path);
			strcat(copy_path," > chmod_file2.dat ");

			if( system(copy_path)==-1 )
			{
				test_warn(log,"Unexpected error: unable to execute system command");
                                case_fail(log,"Failed to change the file access permissions");
                                unlink("chmod_file1.dat");
                                fclose(fp1);
                                test_exit(log,"MMC test exit");
                                exit(-1);
			}		
					
                        if(( fp2 = fopen("chmod_file2.dat","r" )) == NULL )	/*  Open the redirected file */
                        {
                                test_warn(log,"Unexpected error: unable to open the file");
                               	case_fail(log,"Failed to change the file access permissions");
                                flag=FAIL;
				unlink("chmod_file1.dat");                               
				fclose(fp1);
                                break;
                        }

                        while(fgets(line,NINETY_BYTES,fp2)!=NULL)	/* get  a line form the file */
                        {
                                sscanf(line,"%s ",permissions_ch);

                        } /* end of while loop */
			

			if(strcmp(permissions,permissions_ch)==0)	/* Verify the permissions are changed */
			{
				test_error(log,"Unexpected error: File access permissions did not change");
                               	case_fail(log,"Failed to change the file access permissions");
                                flag=FAIL;
				unlink("chmod_file1.dat");
	                        unlink("chmod_file2.dat");
				fclose(fp1);
	                        fclose(fp2);
                                break;
			}
	
			case_pass(log,"File access permissions changed successfully"); 
                        flag=PASS;
			unlink("chmod_file1.dat");
			unlink("chmod_file2.dat");		
			fclose(fp1);
			fclose(fp2);
			
                        break;


		/************************* Create a directory ****************************************/
                case 9:
			case_start(log,"Running test 9: Create a directory in MMC card");

                        if(!check_path)		/* Verify the path is correct */ 
                        {
				test_warn(log,"Specified path is incorrect");
				case_fail(log,"Failed to create the directory");
                                flag=FAIL;
       	                        break;					
			}

			/* Create the directory */
                        if(mkdir(path,S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IXOTH)==-1)
                        {	
				 case_fail(log,"Failed to create the directory");
                                 flag=FAIL;
                                 break;				
                        }
			 
			case_pass(log,"Creation of the directory is successful");
                        flag=PASS;
                        break;

		/************************ Copy a file into directory ***********************************/
                case 10:
			case_start(log,"Running test 10: Copy a file into a directory in MMC card"); 

                        if((fdr=open(first_arg,O_RDONLY))==-1)		/* Open the file which is to be copied */
                        {
				test_warn(log,"Specified path is incorrect");
				test_error(log,"Failed to open the source file"); 	
				case_fail(log,"Failed to copy a file into directory");
                                flag=FAIL;
                                break;

                        }

                        strcpy(temp_path,first_arg);

                        temp=first_arg;

                        while((hold=strtok(temp,delim))!=NULL)
                        {
                                temp=NULL;
                                strcpy(filename,hold);

                        } /* end of while loop */

                        if(strstr(second_arg,"/")!=NULL)	/* Verify the destination path to be copied */
                        {
                                if(strstr(second_arg,MOUNT_PATH)==NULL)
                                {
					test_warn(log,"Specified path is incorrect");
                                        case_fail(log,"Failed to copy a file into directory");
	                                flag=FAIL;
					close(fdr);
                                        break;                               

                                }
                                else
                                {
                                        strcpy(path,second_arg);
                                }
                        }

                        else
                        {
                                strcpy(path,MOUNT_PATH);
                                strcat(path,"/");
                                strcat(path,second_arg);
                        }

			strcpy(string,path);
                        strcat(string,"/");
                        strcat(string,filename);

			/* Open a file at the destination  */
                        if((fdw=open(string,O_RDWR|O_CREAT ,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH ))==-1)
                        {
				test_error(log,"Failed to open the destination file");
				case_fail(log,"Failed to copy a file into directory");
                                flag=FAIL;
				close(fdr);
	                        break;
				
                        }

                        while(read(fdr,&readChar,1)==1)		/* read  from source file copy to the destination file */
                        {

                          if(write(fdw,&readChar,1)!=1)
                          {
				 test_warn(log,"Write to destination file failed");
				 case_fail(log,"Failed to copy a file into directory");
                                 flag=FAIL;
                                 close(fdw);
                                 close(fdr);								
				 break;
                          }

                        }/* end of WHILE LOOP */


                        if( ( dp = opendir (path)) == NULL )		/* Open the destination directory */
                        {
				test_warn(log,"Cannot open specified directory");	
				case_fail(log,"Failed to copy a file into directory");
                                flag=FAIL;
                                close(fdw);
                                close(fdr);
				break;
                        }

                        while( (dirp = readdir(dp) )!= NULL)  	     /* Verify the copied file is present in the directory */
                        {
                          if(strcmp(filename,dirp->d_name)==0)
                          {
				unlink("comp");
			
				strcpy(compare,"cmp ");
				strcat(compare,temp_path);
				strcat(compare," ");
				strcat(compare,string);
				strcat(compare," > ");
				strcat(compare,"comp");	
			
				if(system(compare)==-1)			/* compare the files */
				{
					test_warn(log,"Unexpected Error: cannot execute system command");
                                        case_fail(log,"Failed to copy a file into directory");
					test_exit(log,"MMC test exit");
	                                exit(-1);
				}
				
				if(( cfp = open("comp",O_RDONLY )) == -1 )
				{	
					test_warn(log,"Unexpected Error: Cannot open the file");
					case_fail(log,"Failed to copy a file into directory");
	                                flag=FAIL;
					break;
				}	
				else
				{
					if(read(cfp,&read_buf,1)!=1)			
					{
						case_pass(log,"Copying of file into directory is successful");
			                        flag=PASS;
	              			        unlink("comp");
						break;
					}
					else{
		                                flag=FAIL;
					}
				}
			
                          }/* if */
                        }/* end of while loop */
		

                        if( dirp == NULL )
                        {	
				test_error(log,"Unexpected Error: Copied file not found");
				case_fail(log,"Failed to copy a file into directory");
                                flag=FAIL;
				break;
                        }

                        closedir(dp);
                        close(fdr);
                        close(fdw);		
			break;	


		 /*********************** Modifying the file create date  ******************************************/
                case 11:
                        case_start(log,"Running test 11: Modifying the file create date in MMC card");

                        if(!check_path)		/* Verify the path is correct */
                        {
				test_warn(log,"Specified path is incorrect");
                                case_fail(log,"Failed to modify the file creation date");
                                flag=FAIL;
				break;
			}

			if ( (fd = open(path,O_RDONLY) ) == -1 ) 	/* Check if the file is present */
		        {
				test_warn(log,"Specified file does not exist");				
                        	case_fail(log,"Failed to modify the file creation date");
                                flag=FAIL;
                                break;

       			}

			unlink("touch.dat");

			memset(arg_year,0,NINETY_BYTES);
		        memset(arg_month,0,NINETY_BYTES);
		        memset(arg_day,0,NINETY_BYTES);

			if(strlen(second_arg)!=TWELVE_BYTES)		/* Check for valid data that is to be modified */
			{	
				test_warn(log,"incorrect values specified for date change request");
                                case_fail(log,"Failed to modify the file creation date");
                                flag=FAIL;
                                break;

			}

		        strncpy(arg_year,second_arg,4);			
		        
		        strncpy(arg_month,second_arg+4,2);
		        i_month=atoi(arg_month);		             

		        strncpy(arg_day,second_arg+6,2);		       	

			strcpy(string,"touch -t ");
		        strcat(string,second_arg); /* date time */
		        strcat(string," ");
		        strcat(string,path); /* file name */

		        if( system(string) ==-1 )			/* Modify the data */
			{
				test_warn(log,"Unexpected Error: Could not execute touch command for date change");
                                case_fail(log,"Failed to modify the file creation date");
				close(fd);
				test_exit(log,"MMC test exit");
                                exit(-1);                       
			}

			strcpy(file_list,"ls --full-time ");	
		        strcat(file_list,path);
      			strcat(file_list," > touch.dat");	

		        if(system(file_list)==-1)			/* Redirect the file time and date */ 
        		{        
                		test_warn(log,"Unexpected Error: Could not read file attributes");
                                case_fail(log,"Failed to modify the file creation date");
				close(fd);
				test_exit(log,"MMC test exit");
                                exit(-1);
        		}

			if(( fp1 = fopen("touch.dat","r" )) == NULL )	/* Open the redirected file */
		        {
		                test_warn(log,"Unexpected Error: could not open the file");
                		case_fail(log,"Failed to modify the file creation date");
                                flag=FAIL;
				close(fd);
                                break;
        		}	

			while(fgets(line,NINETY_BYTES,fp1)!=NULL)	/* get a line from touch.dat */
			{
				sscanf(line,"%s %s %s %s %s %s %s %s %s %s %s ",permissions,type,user,cuser,bytes,week,month,date,time,year,filep);
	
			} /* end of while loop */

			switch(i_month)					/* Convert the  variable month from int to char */
		        {
                		case 1:
                                	strcpy(arg_month,"Jan");
	                                break;

        		        case 2:
                        	        strcpy(arg_month,"Feb");
                                	break;

		                case 3:
                	                strcpy(arg_month,"Mar");
                        	        break;

               			case 4:
                                	strcpy(arg_month,"Apr");
                               		 break;

		                case 5:
                	                strcpy(arg_month,"May");
                        	        break;

                		case 6:
                                	strcpy(arg_month,"Jun");
                                	break;

		                case 7:
                	                strcpy(arg_month,"Jul");
                        	        break;

               			case 8:
                                	strcpy(arg_month,"Aug");
                                	break;

		                case 9:
                	                strcpy(arg_month,"Sep");
                        	        break;

		                case 10:
                	                strcpy(arg_month,"Oct");
                        	        break;


		                case 11:
                	                strcpy(arg_month,"Nov");
                        	        break;

		                case 12:
		                       strcpy(arg_month,"Dec");
	                                break;

        		        default :
                        	        break;

			}/* end of switch */


			/* Compare the modified date and time to the input data */
			printf("arg_year=%s\n",arg_year);
			printf("arg_month=%s\n",arg_month);
			printf("arg_day=%s\n",arg_day);
			printf("year=%s\n",year);
			printf("month=%s\n",month);
			printf("date=%s\n",date);
		     if( (strcmp(arg_year,year)!=0)   || (strcmp(arg_month,month)!=0) || (strcmp(arg_day,date)!=0)  )
		     {
				
                                test_error(log,"File attributes are not set as expected");
                                case_fail(log,"Failed to modify the file creation date");
                                flag=FAIL;
				unlink("touch.dat");
				close(fd);
				fclose(fp1);
                                break;            
        	     }		
	             else
		     {
				case_pass(log,"Modifying the file create date successful");
                                flag=PASS;
    		     }

			unlink("touch.dat");
			close(fd);
			fclose(fp1);
			break;


  		/*********************** Renaming of directory ******************************************/
                case 12:
			case_start(log,"Running test 12: Renaming of a directory in MMC card"); 

                        if(!check_path)		/* Verify the path is correct */
                        {
				test_warn(log,"Specified path is incorrect");
                                case_fail(log,"Failed to rename the directory");
                                flag=FAIL;
                                break;
 			}

                        strcpy(string,MOUNT_PATH);
                        strcat(string,"/");
                        strcat(string,second_arg);

                        if(rename(path,string)==-1)		/* Rename the directory */
                        {
				test_warn(log,"Specified Directory does not exist");
				case_fail(log,"Failed to rename the directory");
                                flag=FAIL;
				break;			
                        }

			case_pass(log,"Renaming of directory successful");  
                        flag=PASS;
                        break;		
		
                /***************** Move the file into directory  ***************************************/		
        	   case 13:
			
                        case_start(log,"Running test 13: Move a file into a directory in MMC card");


                        if((fdr=open(first_arg,O_RDONLY))==-1)		/* Open the file to be moved */
                        {
				test_warn(log,"Specified path is incorrect");
                                test_error(log,"Opening the source file failed");
                                case_fail(log,"Failed to move a file into a directory");
                                flag=FAIL;
                                break;
                        }

                        strcpy(temp_path,first_arg);

                        temp=first_arg;

                        while((hold=strtok(temp,delim))!=NULL)
                        {
                                temp=NULL;
                                strcpy(filename,hold);

                        } /* end of while loop */

			if(strstr(second_arg,"/")!=NULL)		/* Verify the destination path */
                        {
                                if(strstr(second_arg,MOUNT_PATH)==NULL)
                                {
                                        test_warn(log,"Incorrect path Specified");
                                	case_fail(log,"Failed to move a file into a directory");
	                                flag=FAIL;
					close(fdr);
                                        break;
                                }
                                else
                                {
                                        strcpy(path,second_arg);
                                }
                        }

                        else
                        {
                                strcpy(path,MOUNT_PATH);
                                strcat(path,"/");
                                strcat(path,second_arg);
                        }

                        strcpy(string,path);
                        strcat(string,"/");
                        strcat(string,filename);

			/* Create a new file at the specified destination */
 
                        if((fdw=open(string,O_RDWR|O_CREAT ,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH ))==-1)
                        {

                                test_error(log,"Opening destination file failed");
                               	case_fail(log,"Failed to move a file into a directory");
                                flag=FAIL;
				close(fdr);
                                break;
                        }

		        while(read(fdr,&readChar,1)==1)		/* Read from the source file and copy to the destination */
                        {

                          if(write(fdw,&readChar,1)!=1)
                          {
                                 test_warn(log,"Write failed");
                               	 case_fail(log,"Failed to move a file into a directory");
                                 flag=FAIL;
                                 close(fdw);
	                         close(fdr);
 				 break;
                          }

                        }/* WHILE LOOP */


                        if( ( dp = opendir (path)) == NULL )		/* Open the directory where the file is moved */
                        {
                                test_warn(log,"Cannot open specified directory");
                               	case_fail(log,"Failed to move a file into a directory");
                                flag=FAIL;
                                close(fdw);
                                close(fdr);
				break;

                        }
			
                        if(unlink(temp_path)==-1)			/* Delete the source file */
			{
				test_error(log,"unexpected Error: unable to move the file"); 
                               	case_fail(log,"Failed to move a file into a directory");
                                flag=FAIL;
                                close(fdw);
                                close(fdr);
				break;
                        }

                        while( (dirp = readdir(dp) )!= NULL )	      /* Verify the file is moved to destination directory */
                        {
                          if(strcmp(filename,dirp->d_name)==0)
                          {
                                case_pass(log,"File is moved into directory successfully");
	                        flag=PASS;
                                break;
                          }
                        } /* end of while loop */


                        if( dirp == NULL )
                        {
				test_error(log,"Unexpected Error: Destination not found");
                               	case_fail(log,"Failed to move a file into a directory");
                                flag=FAIL;
                        }

                        closedir(dp);
                        close(fdr);
                        close(fdw);

                        break;

		/******************** Change the directory access permissions ****************************/
                case 14:
                        case_start(log,"Running test 14 :Change the directory access permissions in MMC card");


                        if(!check_path)			/* Verify the path is correct */
                        {
				test_warn(log,"Specified path is incorrect");
                                case_fail(log,"Failed to change the Directory access permissions");
                                flag=FAIL;
                                break;
			}

                        unlink("chmod_file1.dat");


                        strcpy(string,"ls -ld ");
                        strcat(string,path);
                        strcat(string," > chmod_file1.dat ");

                        if(system(string)==-1)		 /* Redirect the directory access permissions to a new file */
                        {
                                test_warn(log,"Unexpected Error: could not execute system command");
                                case_fail(log,"Failed to change the Directory access permissions");
				test_exit(log,"MMC test exit");
                                exit(-1);
                        }

                        if(( fp1 = fopen("chmod_file1.dat","r" )) == NULL )	 /* Open the redirected file */
                        {
                                test_warn(log,"Unexpected Error: could not open file");
                                case_fail(log,"Failed to change the Directory access permissions");
                                flag=FAIL;
                                break;
                        }

                        while(fgets(line,NINETY_BYTES,fp1)!=NULL)		/* Get a line from redirected file */
                        {
                                sscanf(line,"%s ",permissions);

                        }

		  	unlink("chmod_file2.dat");			 

                        strcpy(temp_path,"chmod ");
                        strcat(temp_path,second_arg);
                        strcat(temp_path," ");
                        strcat(temp_path,path);

                        if(system(temp_path)==-1)		 /* Change the permissions of the directory */
                        {
                                test_warn(log,"Unexpected Error: could not execute system command");
                                case_fail(log,"Failed to change the Directory access permissions");
				unlink("chmod_file1.dat");
                                fclose(fp1);
                                test_exit(log,"MMC test exit");
                                exit(-1);

                        }

                        strcpy(copy_path,"ls -ld ");
                        strcat(copy_path,path);
                        strcat(copy_path," > chmod_file2.dat ");

                       	if( system(copy_path)==-1)
			{
                                test_warn(log,"Unexpected Error: could not execute system command");
                                case_fail(log,"Failed to change the Directory access permissions");
                                unlink("chmod_file1.dat");
                                fclose(fp1);
                                test_exit(log,"MMC test exit");
                                exit(-1);
			}

                        if(( fp2 = fopen("chmod_file2.dat","r" )) == NULL )	/* Open the redirected file */
                        {
                                test_warn(log,"Unexpected Error: could not open file");
                                case_fail(log,"Failed to change the Directory access permissions");
                                flag=FAIL;
				unlink("chmod_file1.dat");
                                fclose(fp1);
                                break;
                        }

                        while(fgets(line,NINETY_BYTES,fp2)!=NULL)		/* Get a line form redirected file */
                        {
                                sscanf(line,"%s ",permissions_ch);

                        } /* end of while loop */


                        if(strcmp(permissions,permissions_ch)==0)		/* Verify the permissions are changed */
                        {
				test_error(log,"Unexpected error: Directory access permissions did not change");
                                case_fail(log,"Failed to change the Directory access permissions");
                                flag=FAIL;
				unlink("chmod_file1.dat");
                                unlink("chmod_file2.dat");				
                                fclose(fp1);
                                fclose(fp2);
                                break;
                        }
         
                        case_pass(log,"Directory access permissions changed successfully");
                        flag=PASS;
			unlink("chmod_file1.dat");
                        unlink("chmod_file2.dat");
			fclose(fp1);
                        fclose(fp2);

                        break;


		/************************* Delete a file   *****************************************/
                case 15:
			case_start(log,"Running test 15: Delete a file in MMC card");			

                        if(!check_path)				/* Verify the path is correct */
                        {
				test_warn(log,"Specified path is incorrect");
	                        case_fail(log,"Failed to delete the file");
                                flag=FAIL;
        	                break;
			}
                        if(unlink(path)==-1)			/* Delete the file */			
                        {
				 test_error(log,"File does not exist");
	                         case_fail(log,"Failed to delete the file");
                                 flag=FAIL;
                                 break;

                        }

			case_pass(log,"File deletion successful");			
                        flag=PASS;
                        break;


		/************************ Directory deletion  *****************************************/

                case 16:
			case_start(log,"Running test 16: Directory deletion in MMC card");

                        if(!check_path)				/* Verify the path is correct */
                        {
				 test_warn(log,"Specified path is incorrect");
	                         case_fail(log,"Failed to delete the directory");
                                 flag=FAIL;
        	                 break;
			}

                        if(rmdir(path)==-1)			/* Delete the directory */
                        {
				test_error(log,"Directory does not exists");
	                        case_fail(log,"Failed to delete the directory");
                                flag=FAIL;
                                break;

                        }
                        
			case_pass(log,"Directory deletion is successful");
                        flag=PASS;
                        break;

		

		/********************* Write a large file from SDP to MMC card and delete the file ******************/
		
		case 17:
			
			case_start(log,"Running test 17: Write a large file from SDP to MMC card and delete the file");

			strcpy(temp_path,first_arg);

                        temp=first_arg;

                        while((hold=strtok(temp,delim))!=NULL) 		/* get the filename from the path */
                        {
                                temp=NULL;
                                strcpy(filename,hold);

                        } /* end of while loop */
			
			strcpy(path,second_arg);
			strcat(path,"/");
			strcat(path,filename);			

			strcpy(string,"cp ");
			strcat(string,temp_path);
			strcat(string," ");
			strcat(string,path);

			if(system(string)==-1)                 		/* copy the files */
                        {
	                        test_warn(log,"Unexpected Error: cannot execute system command");
                                case_fail(log,"Failed to copy and delete a large file");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }

                        if(system("sync")==-1)                  /* flush filesystem buffers */                        
			{
                                test_warn(log,"Unexpected Error: cannot execute system command");
                                case_fail(log,"Failed to copy and delete a large file");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }
 			
                        if( ( fdr = open(temp_path,O_RDONLY)) == -1 )            /* Open the source file  */
                        {
                                test_warn(log,"Cannot open specified source file");
                                case_fail(log,"Failed to copy and delete a large file");
                                flag=FAIL;
                                break;
                        }

			if( ( fdw = open(path,O_RDONLY)) == -1 )                /* Open the destination file */
                        {
                                test_warn(log,"Cannot open specified destination file");
                                case_fail(log,"Failed to copy and delete a large file");			
                                flag=FAIL;
                                break;
                        }						

			close(fdr);
			close(fdw);

			unlink("comp.dat");

			strcpy(compare,"diff ");
			strcat(compare,temp_path);
                        strcat(compare," ");
                        strcat(compare,path);
			strcat(compare," > ");
                        strcat(compare,"comp.dat");

			if(system(compare)==-1)                 /* compare the files */
                        {
                                test_warn(log,"Unexpected Error: cannot execute system command");
                                case_fail(log,"Failed to copy and delete a large file");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }

			if(( cfp = open("comp.dat",O_RDONLY )) == -1 )	/* Check for differences in the files */
                        {
                                test_warn(log,"Unexpected Error: Cannot open the file");
                                case_fail(log,"Failed to copy and delete a large file");
                                flag=FAIL;
                                break;
                        }                        
		        
                        if(read(cfp,&read_buf,1)==1)		/* check for any differences in the files */	
                        {
                	        case_fail(log,"Failed to copy and delete a large file");
                                flag=FAIL;
	                        unlink("comp.dat");                                               			
				close(cfp);
				break;
                        }		
			
			if(unlink(path)==0)			/* Delete the copied file */
			{
				case_pass(log,"Copying and deleting of large file is successful");
	                        flag=PASS;
				unlink("comp.dat");
	                        close(cfp);
        	                break;
			}

			case_fail(log,"Failed to copy and delete a large file");		
                        flag=FAIL;
			unlink("comp.dat");	
			close(cfp);
			break;		
	
	
		/**************** Write several small files from SDP to MMC card and delete the files *********/

                case 18:

                        case_start(log,"Running test 18: Write several small files from SDP to MMC card and delete the files");

                        strcpy(temp_path,first_arg);

                        temp=first_arg;

                        while((hold=strtok(temp,delim))!=NULL)		/* Get the directory name */
                        {
                                temp=NULL;
                                strcpy(directory_name,hold);

                        } /* end of while loop */

                        strcpy(string,"cp -R ");
                        strcat(string,temp_path);
                        strcat(string," ");
                        strcat(string,second_arg);		

			strcpy(path,second_arg);
			strcat(path,"/");	
			strcat(path,directory_name);

                        if(system(string)==-1)                          /* copy the files */
                        {
                                test_warn(log,"Unexpected Error: cannot execute system command");
                                case_fail(log,"Failed to copy and delete small files");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }

                        if(system("sync")==-1)                  /* flush filesystem buffers */
                        {
                                test_warn(log,"Unexpected Error: cannot execute system command");
                                case_fail(log,"Failed to copy and delete small files");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }

                        if( ( dp = opendir(temp_path )) == NULL )  	          /* Open the source directory */
                        {
                                test_warn(log,"Cannot open specified source directory");
                                case_fail(log,"Failed to copy and delete small files");
                                flag=FAIL;
                                break;
                        }

                        if( ( dpr = opendir(path)) == NULL )            /* Open the destination directory */
                        {
                                test_warn(log,"Cannot open specified destination directory");
                                case_fail(log,"Failed to copy and delete small files");
                                flag=FAIL;
                                break;
                        }

                        closedir(dp);
                        closedir(dpr);

                        unlink("comp.dat");

                        strcpy(compare,"diff -r ");
                        strcat(compare,temp_path);
                        strcat(compare," ");
                        strcat(compare,path);
                        strcat(compare," > ");
                        strcat(compare,"comp.dat");

                        if(system(compare)==-1)                 /* compare the directories */
                        {
                                test_warn(log,"Unexpected Error: cannot execute system command");
                                case_fail(log,"Failed to copy and delete small files");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }

                        if(( cfp = open("comp.dat",O_RDONLY )) == -1 )	/* Check for differences in the directories */
                        {
                                test_warn(log,"Unexpected Error: Cannot open the file");
                                case_fail(log,"Failed to copy and delete small files");
                                flag=FAIL;
                                break;
                        }

                        if(read(cfp,&read_buf,1)==1)
                        {
                                case_fail(log,"Failed to copy and delete small files");
                                flag=FAIL;
                                unlink("comp.dat");
                                close(cfp);
                                break;
                        }

			strcpy(temp_path,"rm -rf ");        
                        strcat(temp_path,path);
			
			if(system(temp_path)==-1)                 /* Delete the directories */
                        {
                                test_warn(log,"Unexpected Error: cannot execute system command");
                                case_fail(log,"Failed to copy and delete small files");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }

                        if( ( dpr = opendir(path )) == NULL )            /* Try opening the deleted directory */
                        {
				case_pass(log,"Copying and deleting several small files is successful");
	                        flag=PASS;
				unlink("comp.dat");
                                close(cfp);
                                break;
                        }			

                        case_fail(log,"Failed to copy and delete small files");
                        flag=FAIL;
                        unlink("comp.dat");
                        close(cfp);
                        break;

		/******************** Transferring large file between MMC and SDP   **************************/

                case 19:
                        case_start(log,"Running test 19: Transferring large file between MMC and SDP");

                        strcpy(temp_path,first_arg);

                        temp=first_arg;

                        while((hold=strtok(temp,delim))!=NULL)		/* Get the name of the file from path */
                        {
                                temp=NULL;
                                strcpy(filename,hold);

                        } /* end of while loop */

                        strcpy(path,second_arg);
                        strcat(path,"/");
                        strcat(path,filename);

                        strcpy(string,"cp ");
                        strcat(string,temp_path);
                        strcat(string," ");
                        strcat(string,path);


                        if(system(string)==-1)                          /* copy the files */
                        {
                                test_warn(log,"Unexpected Error: cannot execute system command");
                                case_fail(log,"Failed to copy a large file");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }

                        if(system("sync")==-1)                          /* flush filesystem buffers */
                        {
                                test_warn(log,"Unexpected Error: cannot execute system command");
                                case_fail(log,"Failed to copy a large file");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }


                        if( ( fdr = open(temp_path,O_RDONLY)) == -1 )            /* Open the source file */
                        {
                                test_warn(log,"Cannot open specified source file");
                                case_fail(log, "Failed to copy a large file");
                                flag=FAIL;
                                break;
                        }

                        if( ( fdw = open(path,O_RDONLY)) == -1 )                /* Open the destination file */
                        {
                                test_warn(log,"Cannot open specified destination file");
                                case_fail(log,"Failed to copy a large file");
                                flag=FAIL;
                                break;
                        }

                        close(fdr);
                        close(fdw);

                        unlink("comp.dat");

                        strcpy(compare,"diff ");
                        strcat(compare,temp_path);
                        strcat(compare," ");
                        strcat(compare,path);
                        strcat(compare," > ");
                        strcat(compare,"comp.dat");

                        if(system(compare)==-1)                 /* compare the files */
                        {
                                test_warn(log,"Unexpected Error: cannot execute system command");
                                case_fail(log,"Failed to copy a large file");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }

                        if(( cfp = open("comp.dat",O_RDONLY )) == -1 )
                        {
                                test_warn(log,"Unexpected Error: Cannot open the file");
                                case_fail(log,"Failed to copy a large file");
                                flag=FAIL;
                                break;
                        }

                        if(read(cfp,&read_buf,1)==1)		/* Check if any differences in files */
                        {
                                case_fail(log,"Failed to copy a large file");
                                flag=FAIL;
                                unlink("comp.dat");
                                close(cfp);
                                break;
                        }

                        case_pass(log,"Copying a large file is successful");
                        flag=PASS;
                        unlink("comp.dat");
                        close(cfp);

                        break;

	
		/********************* Transferring several small files between MMC and SDP  ****************************/

                case 20:

                        case_start(log,"Running test 20: Transferring several small files between MMC and SDP");

			strcpy(temp_path,first_arg);

                        temp=first_arg;

                        while((hold=strtok(temp,delim))!=NULL)		/* Get the directory name from the path */
                        {
                                temp=NULL;
                                strcpy(directory_name,hold);

                        } /* end of while loop */


                        strcpy(string,"cp -R ");
                        strcat(string,temp_path);
                        strcat(string," ");
                        strcat(string,second_arg);

			strcpy(path,second_arg);
			strcat(path,"/");
                        strcat(path,directory_name);

                        if(system(string)==-1)                          /* copy the files */
                        {
                                test_warn(log,"Unexpected Error: cannot execute system command");
                                case_fail(log,"Failed to copy several small files");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }

                        if(system("sync")==-1)                  /* flush filesystem buffers */
                        {
                                test_warn(log,"Unexpected Error: cannot execute system command");
                                case_fail(log,"Failed to copy several small files");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }

                        if( ( dp = opendir(temp_path)) == NULL )           /* Open the source directory */
                        {
                                test_warn(log,"Cannot open specified source file");
                                case_fail(log,"Failed to copy several small files");
                                flag=FAIL;
                                break;
                        }

                        if( ( dpr = opendir(path)) == NULL )            /* Open the destination directory */
                        {
                                test_warn(log,"Cannot open specified destination file");
                                case_fail(log,"Failed to copy several small files");
                                flag=FAIL;
                                break;
                        }

                        closedir(dp);
                        closedir(dpr);

                        unlink("comp.dat");
			
                        strcpy(compare,"diff -r ");
                        strcat(compare,temp_path);
                        strcat(compare," ");
                        strcat(compare,path);
                        strcat(compare," > ");
                        strcat(compare,"comp.dat");

                        if(system(compare)==-1)                 /* compare the directories */
                        {
                                test_warn(log,"Unexpected Error: cannot execute system command");
                                case_fail(log,"Failed to copy several small files");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }

                        if(( cfp = open("comp.dat",O_RDONLY )) == -1 )	/* Check for differences in the directories */
                        {
                                test_warn(log,"Unexpected Error: Cannot open the file");
                                case_fail(log,"Failed to copy several small files");
                                flag=FAIL;
                                break;
                        }

                        if(read(cfp,&read_buf,1)==1)
                        {
                                case_fail(log,"Failed to copy several small files");
                                flag=FAIL;
                                unlink("comp.dat");
                                close(cfp);
                                break;
                        }

                        case_pass(log,"Copying of several small files is successful");
                        flag=PASS;
                        unlink("comp.dat");
                        close(cfp);
                        break;
	
		/***********************  Attempt to write files to a full MMC  ****************************/

		case 21:
			case_start(log,"Running test 21: Attempt to write files to a full MMC");			
			
			/* Find the card size */

                        unlink("card_size.dat");

                        strcpy(string,"df -hmk ");
                        strcat(string,MOUNT_PATH);
                        strcat(string," > card_size.dat");

                        if( system(string) == -1)     /* Redirect the information about the file system to format_file1.dat */
                        {
                               test_warn(log,"Unexpected Error: could not execute system command");
			       case_fail(log,"Failed to remove files from a full MMC");
                               test_exit(log,"MMC test exit");
                               exit(-1);
                        }

                        if(( fp1 = fopen("card_size.dat","r" )) == NULL )      /* Open the redirected file */
                        {
                                test_warn(log,"Unexpected Error: could not open file");
				case_fail(log,"Failed to remove files from a full MMC");
                                flag=FAIL;
                                break;
                        }

                        if(fgets(line,NINETY_BYTES,fp1)!=NULL)               /* Get the first line from redirected file */
                        {
                                if(fgets(line,NINETY_BYTES,fp1)!=NULL)       /* Get second line from redirected file */
                                {
                                           sscanf(line,"%s %s %s %s %s %s",filesystem,noblocks ,disk_usage,disk_available,percent_use,mounted_on);
				
				card_size=atoi(noblocks);

                                     if(card_size<=0)    /*Verify the device is mounted  */
                                     {
                                           test_warn(log,"Unexpected Error: could not find card size");
					   case_fail(log,"Failed to remove files from a full MMC");
	                                   flag=FAIL;
                                           fclose(fp1);
                                           break;
                                      } /* if stcmp */

                                } /* fget line 2 */
                        } /* fget line 1 */

			fclose(fp1);
			unlink("card_size.dat");
		
			no_times = (card_size/1024) + 4 ;	/* Write for more than card size */

			if(no_times<=0)
			{
				test_warn(log,"Unexpected error: unable to find the file size");
                                case_fail(log,"Failed to remove files from a full MMC");
                                flag=FAIL;
				break;
			}

			strcpy(path,MOUNT_PATH);
                        strcat(path,"/file");

                        if(store_retrieve(path,ONE_MB,log)==-1)
                        {
				test_warn(log,"Unexpected error: unable to create the file");
                                case_fail(log,"Failed to remove files from a full MMC");
                                flag=FAIL;
                                break;
                        }

			for(loop=1;loop<=no_times;loop++)
			{

				strcpy(copy_path,MOUNT_PATH);		/* Destination to be copied */
        	                strcat(copy_path,"/");			
	               	        strcat(copy_path,"file");			

				sprintf(temp_path,"%s%d",copy_path,loop);				

	                        strcpy(string,"cp ");
        	                strcat(string,path);
                	        strcat(string," ");
	                        strcat(string,temp_path);		/* String contains source and destination path */

        	     		if(system(string)==-1)                  /* copy the files */
                	        {
                        	        test_warn(log,"Unexpected Error: cannot execute system command");
	                                case_fail(log,"Failed to remove files from a full MMC");
        	                        test_exit(log,"MMC test exit");
                	                exit(-1);
                        	}	

	                        if(system("sync")==-1)                       /*  flush filesystem buffers */
        	                {
                	                test_warn(log,"Unexpected Error: cannot execute system command");
                        	        case_fail(log,"Failed to remove files from a full MMC");
	                                test_exit(log,"MMC test exit");
        	                        exit(-1);
                	        }
	
			} /*For loop */

                        strcpy(temp_path,"rm -rf ");
                        strcat(temp_path,MOUNT_PATH);
			strcat(temp_path,"/*");

                        if(system(temp_path)==-1)                 /* Delete the directories */
                        {
                                test_warn(log,"Unexpected Error: cannot execute system command");
                                case_fail(log,"Failed to remove files from a full MMC");
                                test_exit(log,"MMC test exit");
                                exit(-1);
                        }

                        if ( ( fd = open(path,O_WRONLY) ) == -1 )               /* open the file for writing */
                        {
				case_pass(log,"Successfully removed files from a full MMC");
	                        flag=PASS;
				break;
			}
		        close(fd); 	
			case_fail(log,"Failed to remove files from a full MMC");
                        flag=FAIL;
			break;
			
	
	
                /****************** Formatting  the MMC card for VFAT file system ********************************/

                case 22:
                        case_start(log,"Running test 22: Formatting the MMC card for VFAT file system ");

			if(strlen(first_arg)==0)
			{
				test_warn(log,"Specify the device name to be formated");
				case_fail(log,"Failed to format the MMC card for VFAT file system");
                                flag=FAIL;
				break;
			}
			
			printf("Formatting in progress............. \n");

                        sleep(15);


			mkdir(MOUNT_PATH,S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IXOTH);

			unlink("format_file1.dat");
			
                        strcpy(string,"df -hmk ");
                        strcat(string,first_arg);
                        strcat(string," > format_file1.dat");


                        if( system(string) == -1)     /* Redirect the information about the file system to format_file1.dat */
                        {
                               test_warn(log,"Unexpected Error: could not execute system command");
                               case_fail(log,"Failed to format the MMC card for VFAT file system");
			       test_exit(log,"MMC test exit");
			       exit(-1);	 	
			}

                        if(( fp1 = fopen("format_file1.dat","r" )) == NULL )      /* Open the redirected file */
                        {
                                test_warn(log,"Unexpected Error: could not open file");
                                case_fail(log,"Failed to format the card for VFAT file system");
                                flag=FAIL;
                                break;
                        }

                        if(fgets(line,NINETY_BYTES,fp1)!=NULL)               /* Get the first line from redirected file */
                        {
                               	if(fgets(line,NINETY_BYTES,fp1)!=NULL)       /* Get second line from redirected file */
                        	{
					   sscanf(line,"%s %s %s %s %s %s",filesystem,noblocks ,disk_usage,disk_available,percent_use,mounted_on);
					                        
		                        if(strcmp(mounted_on,MOUNT_PATH)==0)	/*Verify the device is mounted  */
                		        {
                                		strcpy(temp_path,"umount ");
		                                strcat(temp_path,first_arg);
		
                		                if(system(temp_path)==-1)            /* unmount before formatting */
                                		{
		                                       test_warn(log,"Unexpected Error: could not execute system command");
                		                       case_fail(log,"Failed to format the MMC card for VFAT file system");
                                		       test_exit(log,"MMC test exit");
		                                       fclose(fp1);
               		                               exit(-1);
                                		}/* if system */
                        		} /* if stcmp */
				} /* fget line 2 */
                        } /* fget line 1 */


	                strcpy(path,"mkdosfs ");
        	        strcat(path,first_arg);

                        if(system(path)==-1)		/* format the device for VFAT file system */
                        {
                                test_warn(log,"Unexpected Error: could not execute system command");
                                case_fail(log,"Failed to format the MMC card for VFAT file system");
                                test_exit(log,"MMC test exit");
				fclose(fp1);
                                exit(-1);
                        }

                        strcpy(copy_path,"mount -t vfat ");
                        strcat(copy_path,first_arg);
			strcat(copy_path," ");
                        strcat(copy_path,MOUNT_PATH);

                        if( system(copy_path) == -1)	/* mount the device */
                        {
                                test_warn(log,"Unexpected Error: could not execute system command");
                                case_fail(log,"Failed to format the MMC card for VFAT file system");
				test_exit(log,"MMC test exit");
				fclose(fp1);
                                exit(-1);

                        }

			unlink("format_file2.dat");

                        strcpy(string,"df -hmk ");
                        strcat(string,first_arg);
                        strcat(string," > format_file2.dat");

                        if( system(string) == -1)     /* Redirect the information about the file system to format_file.dat */ 
                        {
                                test_warn(log,"Unexpected Error: could not execute system command");
                                case_fail(log,"Failed to format the MMC card for VFAT file system");
			        test_exit(log,"MMC test exit");
				fclose(fp1);
                                exit(-1);
	
                        }
			
                        if(( fp2 = fopen("format_file2.dat","r" )) == NULL )      /* Open the redirected file */
                        {
                                test_warn(log,"Unexpected Error: could not open file");
                                case_fail(log,"Failed to format the card for VFAT file system");
                                flag=FAIL;
				fclose(fp1);
                                break;
                        }

                        if(fgets(line,NINETY_BYTES,fp2)!=NULL)               /* Get the first line from redirected file */
                        {
                       	       	if(fgets(line,NINETY_BYTES,fp2)!=NULL)       /* Get second line from redirected file */
                        	{
					sscanf(line,"%s %s %s %s %s %s",filesystem, noblocks ,disk_usage,disk_available,percent_use,mounted_on);
					  /*Verify the type , disk space usage and device is mounted on */
		                       if(((atoi(disk_usage))==0)&&(strcmp(mounted_on,MOUNT_PATH)==0))
                		        {
                        		   case_pass(log,"Formatting the MMC card for VFAT file system is successful");
		                           flag=PASS;
		      			   unlink("format_file1.dat");
		                           unlink("format_file2.dat");
                                           fclose(fp1);
                                           fclose(fp2);
			                   break;		
					}
		
				} /* if fget line 2 */
                        } /* if fget line 1 */

			case_fail(log,"Failed to format the MMC card for VFAT file system");			
                        flag=FAIL;
                        unlink("format_file1.dat");
			unlink("format_file2.dat");
			fclose(fp1);
			fclose(fp2);

			sleep(15);

                        break;


                /****************** Formatting  the MMC card for EXT2 fie system ********************************/

                case 23:
		
                	case_start(log,"Running test 23: Formatting the MMC card for EXT2 file system ");

	                if(strlen(first_arg)==0)
                        {
                                test_warn(log,"Specify the device name to be formated");
                                case_fail(log,"Failed to format the MMC card for VFAT file system");
                                flag=FAIL;
                                break;
                        }

                        printf("Formatting in progress............. \n");

                        sleep(15);

                        mkdir(MOUNT_PATH,S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IXOTH);

                        unlink("format_file1.dat");

                        strcpy(string,"df -hmk ");
                        strcat(string,first_arg);
                        strcat(string," > format_file1.dat");

                        if( system(string) == -1)     /* Redirect the information about the file system to format_file1.dat */
                        {
                               test_warn(log,"Unexpected Error: could not execute system command");
                               case_fail(log,"Failed to format the MMC card for EXT2 file system");
			       test_exit(log,"MMC test exit");
			       exit(-1);
                        }

                        if(( fp1 = fopen("format_file1.dat","r" )) == NULL )      /* Open the redirected file */
                        {
                                test_warn(log,"Unexpected Error: could not open file");
                                case_fail(log,"Failed to format the card for EXT2 file system");
                                flag=FAIL;
                                break;
                        }

                        if(fgets(line,NINETY_BYTES,fp1)!=NULL)               /* Get the first line from redirected file */
                        {
                        	if(fgets(line,NINETY_BYTES,fp1)!=NULL)       /* Get second line from redirected file */
                        	{
					  sscanf(line,"%s %s %s %s %s %s",filesystem, noblocks ,disk_usage,disk_available,percent_use,mounted_on);

					   /* Verify the device is mounted  */
		                        if(strcmp(mounted_on,MOUNT_PATH)==0)
                		        {
                                		strcpy(temp_path,"umount ");
		                                strcat(temp_path,first_arg);
		
                		                if(system(temp_path)==-1)            /* unmount before formatting */
                               			{
		                                       test_warn(log,"Unexpected Error: could not execute system command");
                		                       case_fail(log,"Failed to format the MMC card for EXT2 file system");
                                        	       test_exit(log,"MMC test exit");
                	                	       fclose(fp1);
		                                       exit(-1);
                                		} 	
                        		} /* if strsmp */
				} /* if fget line 2 */
                        } /* if fget line 1 */

                        //strcpy(path,"mkfs.ext2 ");
                        strcpy(path,"mke2fs ");
                        strcat(path,first_arg);

                        if(system(path)==-1)            /* format the device for EXT2 filesystem */
                        {
                                test_warn(log,"Unexpected Error: could not execute system command");
                                case_fail(log,"Failed to format the MMC card for EXT2 file system");
                                test_exit(log,"MMC test exit");
                                fclose(fp1);
                                exit(-1);
                        }

                        strcpy(copy_path,"mount -t ext2 ");
                        strcat(copy_path,first_arg);
                        strcat(copy_path," ");
                        strcat(copy_path,MOUNT_PATH);

                        if( system(copy_path) == -1)    /* mount the device */
                        {
                                test_warn(log,"Unexpected Error: could not execute system command");
                                case_fail(log,"Failed to format the MMC card for EXT2 file system");
                                test_exit(log,"MMC test exit");
                                fclose(fp1);
                                exit(-1);

                        }

                        unlink("format_file2.dat");

                        strcpy(string,"df -hmk ");
                        strcat(string,first_arg);
                        strcat(string," > format_file2.dat");

                        if( system(string) == -1)     /* Redirect the information about the file system to format_file.dat */
                        {
                                test_warn(log,"Unexpected Error: could not execute system command");
                                case_fail(log,"Failed to format the MMC card for EXT2 file system");
                                test_exit(log,"MMC test exit");
                                fclose(fp1);
                                exit(-1);

                        }

                        if(( fp2 = fopen("format_file2.dat","r" )) == NULL )      /* Open the redirected file */
                        {
                                test_warn(log,"Unexpected Error: could not open file");
                                case_fail(log,"Failed to format the card for EXT2 file system");
                                flag=FAIL;
                                fclose(fp1);
                                break;
                        }

                        if(fgets(line,NINETY_BYTES,fp2)!=NULL)               /* Get the first line from redirected file */
                        {

	                        if(fgets(line,NINETY_BYTES,fp2)!=NULL)       /* Get second line from redirected file */
                       		{	
					 sscanf(line,"%s %s %s %s %s %s",filesystem,noblocks ,disk_usage,disk_available,percent_use,mounted_on);
		                        /*Verify the type , disk space usage and device is mounted on */
		                      	if((strcmp(mounted_on,MOUNT_PATH)==0))
                			{
					    case_pass(log,"Formatting the MMC card for EXT2 file system is successful");
		                            flag=PASS;
	                 		    unlink("format_file1.dat");
			                    unlink("format_file2.dat");
                        		    fclose(fp1);
                                            fclose(fp2);
                                            break;					
                        		}
                        	} /* if fget line 1 */
			} /* if get line 2 */

			case_fail(log,"Failed to format the MMC card for EXT2 file system");
                        flag=PASS;
                        unlink("format_file1.dat");
			unlink("format_file2.dat");
                        fclose(fp1);
                        fclose(fp2);

			sleep(15);

                        break;


		
		/********************** Attempt to read a file which does not exists ****************/
		case 24:
			case_start(log,"Running test 24: Attempt to read a file which does not exists in MMC card");

			if(!check_path)          /* verify the path is correct */
                        {
                                test_warn(log,"Specified path is incorrect");
                                case_fail(log,"Attempt to read a file which does not exists");                       
                                flag=FAIL;
                                break;
                        }

	                if( (fd = open(path,O_RDONLY ) ) == -1 )        /* open the file for reading */
                        {
                                case_pass(log,"Failed to read the file");
	                        flag=PASS;
                                break;
                        }

			case_fail(log,"Attempt with different file name that does not exist");
                        flag=FAIL;
	
			close(fd);	
			break;


		/********************** Create a directory which already exists *********************/

		case 25:

			case_start(log,"Running test 25: Create a directory which already exists in MMC card");
			
			if(!check_path)         /* Verify the path is correct */
                        {
                                test_warn(log,"Specified path is incorrect");
                                case_fail(log,"Attempt to create the directory which already exists");
                                flag=FAIL;
                                break;
                        }

			if(mkdir(path,S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IXOTH)==-1)
                        {
                                 case_pass(log,"Failed to create the directory");
	                         flag=PASS;
                                 break;
                        }

			case_fail(log,"Attempt to create the directory which already exists");
                        flag=FAIL;
			rmdir(path);			/* If created remove the directory */
                        break;
			

		/**************************  Delete a file twice **************************************/	
		case 26:
			case_start(log,"Running test 26: Delete a file twice in MMC card");
			
			if(!check_path)                         /* Verify the path is correct */
                        {
                                test_warn(log,"Specified path is incorrect");
                                case_fail(log,"Attempt to delete the file twice");
                                flag=FAIL;
                                break;
                        }
			
			if(unlink(path)==-1)                    /* Delete the file */
                        {
                                 test_error(log,"File does not exist");
                                 case_fail(log,"Attempt to delete the file twice");
                                 flag=FAIL;
                                 break;
                        }

			if(unlink(path)==-1)                    /* Delete the file */
                        {
                                 case_pass(log,"Failed to delete the file");
	                         flag=PASS;
                                 break;
                        }

			case_fail(log,"Attempt to delete the file twice");
                        flag=FAIL;
                        break;
	

		default:
                        test_warn(log,"Invalid test case : Exiting");
                        flag=FAIL;
                        break;	

        }/* switch */

	free(string);
        free(path);
        free(copy_path);
        free(temp_path);
        free(filename);
	free(compare);	
	free(arg_year);
        free(arg_month);
 	free(arg_day);
        free(line);
        free(file_list);
	free(directory_name);

	return flag;

}/*function*/

