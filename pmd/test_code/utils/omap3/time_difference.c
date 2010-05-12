#include <stdio.h>
#include <string.h>

void usage(void) {
	printf("USAGE: time_difference Hour(t0) Minute(t0) Second(t0) Hour(t1) Minute(t1) Second(t1) ExpectedDiff\n");
	printf("Get the difference (in seconds) between times Hout(t0):Minute(t0):Second(t0) and Hour(t1):Minute(t1):Second(t1)\n");
	printf("and compare this difference with an expected time difference (in seconds).");
}

int main(int argc, char **argv) {

	int t0_hour=0;
	int t0_minute=0;
	int t0_second=0;
	int t1_hour=0;
	int t1_minute=0;
	int t1_second=0;
	int expected_diff=0;
	int current_diff=0;
	int t0_absolute=0;
	int t1_absolute=0;
	int ret=1;


	if (argc == 8) {
		t0_hour = atoi(argv[1]);
		t0_minute = atoi(argv[2]);
		t0_second = atoi(argv[3]);
		t1_hour = atoi(argv[4]);
		t1_minute = atoi(argv[5]);
		t1_second = atoi(argv[6]);
		expected_diff = atoi(argv[7]);

		printf("Initial Time: %02d:%02d:%02d\n", t0_hour, t0_minute, t0_second);
		printf("Final Time:   %02d:%02d:%02d\n", t1_hour, t1_minute, t1_second);

		t0_absolute = 60*60*t0_hour+60*t0_minute+t0_second;
		t1_absolute = 60*60*t1_hour+60*t1_minute+t1_second;

		current_diff = t1_absolute-t0_absolute;
		if (current_diff == expected_diff) {
			printf("Difference of expected and current times: PASS\n");
			ret = 0;
		}
		else {
			printf("Difference of expected and current times: FAIL\n");
			ret = 1;
		}
	}
	else {
		printf("Incorrect number or arguments\n");
		usage();
		ret = 1;
	}

	return ret;
}
