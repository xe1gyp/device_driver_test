#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#define MAX_APP 	2
#define MAX_LENGTH 	32
static pid_t appPidList[MAX_APP] = { -1, -1};

static char appList[MAX_APP][MAX_LENGTH] =
  {
    "/bin/ts_test",
    "/bin/ts_calibrate"
  };

int  main(void)
{
	int i=0;
        int which = PRIO_PROCESS;
	int priority=-20;
	int retn=-1;

	for(i = 0; i<MAX_APP; i++ )
    	{
      		if((appPidList[i] = fork()) < 0)
		{
	  		printf("fork failed for %s",appList[i]);
	  		exit(-1);		
		}
      		else
		{
	  		if(appPidList[i] == 0) //child process
	    		{
				appPidList[i]=getpid();
	      			printf("Spawned in main ProcessName=%s its ret=%d\n", appList[i],appPidList[i]);
	      			if(execlp("/bin/sh", "sh", "-exec",  appList[i], (char *)0) < 0)
				{
		  			printf("execlp failed for %s", appList[i]);
		  			exit(-1);
				}
				else
				{
					printf("application %d --%s is successfully launched \n",i,appList[i]);
				} 
			}
	    	}
		sleep(3);
	}
/* Setting the second process at highest priority */
	retn = setpriority(which, appPidList[i-1], priority);
	if(retn == -1)
		printf("Error : while setting the priority of the PID =%d\n",appPidList[i-1]);
	else
		printf("Success : Setting the priority of the PID =%d\n",appPidList[i-1]);
	return 1;
	
}	
