
#include <stdio.h>
#include "testlog.h"

int
main()
{
    testlog *l;
    /* test 3 */
    l = test_create();
    addfilebyname(l, "testout3.dat", VERBOSE);
    test_init(l, "test case #3");
    test_info(l, "this is a test3 info mesage");
    test_warn(l, "this is going to fail");
    test_fail(l, "Test 3 failed, cause it is supposed to test test_fail");
    free(l);

    return -1;
}
