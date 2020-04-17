#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "usage : %s <datafile>\n", argv[0]);
		exit(1);
	}

	struct stat statbuf;
	struct timeval startTime, endTime;
	int i, fd, record_num, diffTime, diff;
	FILE *fp;
	char buf[100];

	if ((fp = fopen(argv[1], "r")) < 0) {
			fprintf(stderr, "open error for %s\n", argv[1]);
			exit(1);
			}

	fseek(fp, 0, SEEK_END);
	record_num = ftell(fp) / 100;

	gettimeofday(&startTime, NULL);
	for (i = 0; i < record_num; i++) {
		if ((fd = fread(buf, 100, 1, fp) > 0)) {
			fprintf(stderr, "read error!\n");
			exit(1);
		}
		printf("%d\n", fd);
	}
	gettimeofday(&endTime, NULL);
	diffTime = (endTime.tv_sec - startTime.tv_sec) * (1e+6);
	diffTime += (int)(endTime.tv_usec - startTime.tv_usec);

	fclose(fp);

	printf("#records : %d timecost : %d : %d us\n", record_num, (int)(diffTime / (1e+6)), diffTime);

		return 0;
}
