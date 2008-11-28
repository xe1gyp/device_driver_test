/*
 * Jean II '99
 *
 * Send ultra info.
 * Open a Ultra socket and send one packet...
 *
 * Note : Ultra can carry only 382 bytes / packet, and this including
 * the ultra header (the UPID). As we use a one byte UPID, that's 381 bytes.
 */

#include <sys/socket.h>
#include <sys/types.h>

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

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
  unsigned char		buf[382];	/* Send buffer */
  int			actual;

  /* Initialise with a dummy string */
  strcpy(buf, "Ultra-Test");

  /* Look for command line args */
  if(argc > 1)
    {
      strncpy(buf, argv[1], 380);
      buf[380] = '\0';
    }

  /* Create socket  - Ultra type */
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
  printf("Ready to send on Ultra-PID %d\n", UPID);

  /* Send a packet including the trailing '\0' */
  actual = send(fd, &buf, strlen(buf) + 1, 0);
  printf("Sent Ultra packet ``%s''\n", buf);

  close(fd);
  return(0);
}















