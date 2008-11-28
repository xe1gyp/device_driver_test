/*
 * Jean II '99
 *
 * Receive ultra info.
 * Open a Ultra socket and wait for packets...
 */

#include <sys/socket.h>
#include <sys/types.h>		/* Not needed */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>		/* Not needed */

#include <linux/types.h>
#include <linux/irda.h>

#ifndef AF_IRDA
#define AF_IRDA 23
#endif /* AF_IRDA */

/*
 * This is the Ultra protocol ID we are currently using (7 bits)
 * Currently, IrDA-Ultra defines only UPID 0x1 for Obex and 0x2 for IrWW.
 * Maybe you should use a bigger number to avoid clashes with future
 * standardisations...
 */
#define UPID	0x3

int
main(int	argc,
     char *	argv[])
{
  struct sockaddr_irda	self;		/* Socket address */
  int			fd;		/* The socket itself */
  unsigned char		buf[382];	/* Receive buffer */
  int			actual;

  /* Create socket - Ultra type */
  fd = socket(AF_IRDA, SOCK_DGRAM, 1);
  if(fd < 0)
    {
      perror("Creating socket");
      return(-1);
    }

  /* Bind to the specific Ultra PID */
  self.sir_family = AF_IRDA;
  self.sir_lsap_sel = UPID;
  if(bind(fd, (struct sockaddr*) &self, sizeof(struct sockaddr_irda)))
    {
      perror("Connect");
      return(-1);
    }

  /* Receive packets */
  printf("Waiting for Ultra packets on UPID %d...\n", UPID);
  while(1)
    {
      /* Block on read */
      actual = recv(fd, &buf, sizeof(buf), 0);
      buf[actual] = '\0';	/* Just to avoid crash on incorrect data */
      printf("Received packet ``%s''\n", buf);
    }

  /* Never reached */
  close(fd);
  printf("Disconnected!\n");
  return(0);
}















