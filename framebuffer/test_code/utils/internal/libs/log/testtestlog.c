
#include <stdio.h>
#include "testlog.h"

int
main()
{
    testlog *l = test_create();
    addfilebyname(l, "testout.dat", VERBOSE);
    test_init(l, "starting");
    test_warn(l, "this is a warning");
    test_info(l, "info message");
    test_error(l, "this is an error");
    case_start(l, "starting case");
    case_pass(l, "test case passed");
    printf("after case_fail, next exit\n");
    test_exit(l, "test exit");
    free(l);
    
    return 0;
}
