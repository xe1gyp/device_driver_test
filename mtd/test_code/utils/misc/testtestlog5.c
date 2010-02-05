#include <stdio.h>
#include <string.h>
#include "testlog.h"

#define LOGSZ 512

int
main(int c, char *argv[])
{
    testlog *l;
    char logbuf[LOGSZ];

//    printf("argv[1] = %s\n", argv[1]);

	l = test_create();
	addfilebyname(l, argv[1], VERBOSE);

	if (!strcasecmp(argv[2], "ADDFILE")) {
		addfilebyname(l, argv[1], VERBOSE);
	}
	else if (!strcasecmp(argv[2], "INIT")) {
		test_init(l, argv[3]); ////arg is case id
	}
	else if (!strcasecmp(argv[2], "INFO")) {
		l->logging = 1;
		strcpy(logbuf, *(argv+3));
		test_info_f(l, "this tests %s", logbuf);
	}
	else if (!strcasecmp(argv[2], "START")) {
		l->logging = 1;
		case_start(l,argv[3]);//arg is case id
	}
	else if (!strcasecmp(argv[2], "SCEN")) {
		l->logging = 1;
		case_scenario(l, argv[3]);//arg is case id
	}
	else if (!strcasecmp(argv[2], "VALID")) {
		l->logging = 1;
		case_validation(l, argv[3]);//arg is case id
	}
	else if (!strcasecmp(argv[2], "DESC")) {
		l->logging = 1;
		strcpy(logbuf, *(argv+3));
		case_description_f(l, "%s", logbuf);
	}
	else if (!strcasecmp(argv[2], "PASS")) {
		l->logging = 1;
		l->case_start = 1;
		case_pass(l, argv[3]);//arg is case id
	}
	else if (!strcasecmp(argv[2], "FAIL")) {
		l->logging = 1;
		l->case_start = 1;
		case_fail(l, argv[3]);//arg is case id
	}
	else if (!strcasecmp(argv[2], "FAIL-RES")) {
		l->logging = 1;
		test_fail(l, "Test  failed as it is supposed to");

	}
	else if (!strcasecmp(argv[2], "TEXIT")) {
		l->logging = 1;
		test_exit(l, argv[3]); //arg is case id
	}

    free(l);


    return 0;
}
