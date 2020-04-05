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
	int i, fd, record_num, diffTime;
	char buf[100];

	if ((fd = open(argv[1], O_RDONLY)) < 0) {
			fprintf(stderr, "open error for %s\n", argv[1]);
			exit(1);
			}

	fstat(fd, &statbuf);
	record_num = statbuf.st_size / 100;

	gettimeofday(&startTime, NULL);
	for (i = 0; i < record_num; i++) {
		if (read(fd, buf, 100) != 100) {
			fprintf(stderr, "read error!\n");
			exit(1);
		}
	}
	gettimeofday(&endTime, NULL);
	diffTime = (endTime.tv_sec - startTime.tv_sec) * (1e+6);
	diffTime += (int)(endTime.tv_usec - startTime.tv_usec);

	close(fd);

	printf("#records : %d timecost : %d us\n", record_num, diffTime);

		return 0;
}
