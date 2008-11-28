#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include <irda.h>

int ias_query(int fd, int daddr, char *class, char *attrib)
{
	struct irda_ias_set query;
	int len;

	len = sizeof(struct irda_ias_set);

	printf("Querying for %s:%s\n", class, attrib);

	strncpy(query.irda_class_name, class, IAS_MAX_CLASSNAME);
	strncpy(query.irda_attrib_name, attrib, IAS_MAX_ATTRIBNAME);
	if(daddr)
		query.daddr = daddr;

	if (getsockopt(fd, SOL_IRLMP, IRLMP_IAS_QUERY, &query, &len)) {
		perror("getsockopt");
		exit(-1);
	}

	if (len > 0) {
		printf("Success2\n");

		switch (query.irda_attrib_type) {
		case IAS_INTEGER:
			printf("%s:%s=%d\n", class, attrib, 
			       query.attribute.irda_attrib_int);
			break;
		case IAS_OCT_SEQ:
			printf("got octseq\n");
			break;
		case IAS_STRING:
			printf("%s:%s=%s\n", class, attrib, 
			       query.attribute.irda_attrib_string.string);
			break;
		case IAS_MISSING:
			printf("Missing!\n");
			break;
		default:
			printf("Unknown type %d!\n", query.irda_attrib_type);
			break;
		}
	}
	return 0;
}

/*
 * Function main (argc, )
 *
 *    
 *
 */
int main(int argc, char *argv[])
{
	int fd;
	int iraddr;

	/* Create socket */
	fd = socket(AF_IRDA, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket");
		exit(-1);
        }
	/* Check args */
	if(argc < 4) {
		fprintf(stderr, "Usage : %s iraddr class attrib\n", argv[0]);
		return 0;
	}
	/* Convert IrDA address to binary */
	if(sscanf(argv[1], "%X", &iraddr) != 1) {
		fprintf(stderr, "Invalid address: \"%s\"...\n", argv[1]);
		return 0;
	}
	ias_query(fd, iraddr, argv[2], argv[3]);

	return 0;
}
