
#include <stdio.h>
#include "testlog.h"

int
main()
{
    testlog *l;
    
    /* test 4 */
    l = test_create();
    addfilebyname(l, "testout4.dat", VERBOSE);
    test_init_f(l, "test case #%d",4);
    test_info_f(l, "this tests the *_f macros %d", 4);
    case_start_f(l,"testtestlog%d", 4);
    case_pass_f(l, "this case passes %s %d", "case", 4);
    test_fail_f(l, "Test %d failed, cause it is supposed to", 4);
    free(l);

    return 0;
}
