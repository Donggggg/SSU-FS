#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	int fd, n;
	char record[100];
	char *fname = "s1.dat";

	if ((fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0600)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	scanf("%d", &n);

	while(n--)
	if (write(fd, record, sizeof(record)) != 100) {
		fprintf(stderr, "write error!\n");
		exit(1);
	}
	 
	close(fd);
	exit(0);
}


