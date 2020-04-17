#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

#define SUFFLE_NUM	10000	// 이 값은 마음대로 수정할 수 있음

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "usage : %s <data file>\n", argv[0]);
		exit(1);
	}

	struct stat statbuf;
	struct timeval startTime, endTime;
	int i, fd, record_num, diffTime;
	int *read_order_list;
	int num_of_records;
	char buf[100];

	if ((fd = open(argv[1], O_RDONLY)) < 0) {
			fprintf(stderr, "open error for %s\n", argv[1]);
			exit(1);
	}

	if (fstat(fd, &statbuf) < 0) {
		fprintf(stderr, "fstat error!\n");
		exit(1);
	}

	num_of_records = statbuf.st_size / 100;
	read_order_list = malloc(sizeof(int) * num_of_records);

	GenRecordSequence(read_order_list, num_of_records);

	gettimeofday(&startTime, NULL);
	for (i = 0; i < num_of_records; i++) {
		if (lseek(fd, read_order_list[i] * 100, SEEK_SET) < 0) {
				fprintf(stderr, "lseek error\n");
				exit(1);
		}
		if (read(fd, buf, 100) != 100) {
			fprintf(stderr, "read error\n");
			exit(1);
		}
	}
	gettimeofday(&endTime, NULL);
	diffTime = (endTime.tv_sec - startTime.tv_sec) * (1e+6);
	diffTime += (int)(endTime.tv_usec - startTime.tv_usec);

	close(fd);
	
	printf("#records : %d timecost : %d : %d us\n", num_of_records, (int)(diffTime / (1e+6)), diffTime);

	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for (i = 0; i < n; i++)
	{
		list[i] = i;
	}

	for(i = 0; i < SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}

	return;
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

	return;
}
