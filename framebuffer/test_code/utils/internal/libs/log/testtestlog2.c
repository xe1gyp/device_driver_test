
#include <stdio.h>
#include "testlog.h"

int
main()
{
    /* test 2 */
    testlog *l = test_create();
    addfilebyname(l, "testout2.dat", VERBOSE);
    test_init(l, "test case #2");
    test_info(l, "this is a test2 info mesage");
    test_warn(l, "this is going to exit with an error");
    test_error_exit(l, "I told you so!!!");
    free(l);


    return -1;
}
