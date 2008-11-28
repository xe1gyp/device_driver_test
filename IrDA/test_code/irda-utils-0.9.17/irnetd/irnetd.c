/*
 *	    Automatic PPP connection using irNET
 *
 *		Luiz Magalhaes - HPL '00 (luiz_magalhaes@hp.com)
 *		Jean Tourrilhes - HPL '00 <jt@hpl.hp.com>
 *
 *	this program is available under the GNU Public License
 *	you are welcome to use and change this program as long as
 *	you keep this notice and make your code available under
 *	the GPL
 */

/* This program listens to the events in /dev/irnet
 * and creates a PPP connection if it detects a 
 * connection request
 */

/* History :
 * v1.0 : Luiz : just launch ppp
 * v1.1 : Luiz : add IrDA address selectivity
 * v1.2 : Jean : Add command line args and more comments
 * v1.3 : jean : Add "connect-delay 0 idle 10" by default...
 */

/* Usage :
 * If pppd support the nolock option, just run irnetd.
 * Otherwise, run : <irnetd noauth>
 * You can play with more options as you wish...
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  char line[255];
  char cmd1[] = "pppd /dev/irnet 9600 local ";
  char cmd2[] = "noauth nolock connect-delay 0 idle 10 ";
  char cmd3[] = "connect \"echo addr \0";
  char cmd[255];
  char *endp;
  char aux[10]= "01235678\"\0";
  int i;
  FILE *irnet;

  /* Start building our command */
  strcpy(cmd, cmd1);

  /* Check if we have some args to add */
  if(argc == 1)
    {
      /* No arguments - use defaults */
      strcat(cmd, cmd2);
    }
  else
    {
      /* Put args in the ppp command */
      for(i = 1; i < argc; i++)
	{
	  strcat(cmd, argv[i]);
	  strcat(cmd, " ");
	}  
    }

  /* Terminate the fixed part of the command */
  strcat(cmd, cmd3);
  endp = cmd + strlen(cmd);
  strcat(cmd, aux);
  printf("The command is [%s]\n", cmd);

  /* Open IrNET event channel */
  irnet = fopen("/dev/irnet","r");
  if(irnet == NULL)
    {
      perror("Error opening /dev/irnet: ");
      return(-1);
    }

  /* Loop forever */
  while(1)
    {
      /* Wait for one event */
      fgets(line, 255, irnet);

      /* If we have a request...
       * Note : we could also trigger on discovery events, but that would
       * cause problems in many cases, so we keep to request only */
      if(strncmp(line,"Req",3) == 0)
	{
	  /* Copy over the IrDA address */
	  for(i=0; i<8; i++)
	    endp[i] = line[i+13];
	  /* Run pppd to handle connection */
	  if(system(cmd) < 0)
	    perror("irnetd: ");
	}
    }
}


