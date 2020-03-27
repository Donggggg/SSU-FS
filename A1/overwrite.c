#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char* argv[])
{
	if (argc < 4){
		fprintf(stderr, "argv number error!\n");
		exit(1);
	}

	char* filename = argv[1];
	int offset = atoi(argv[2]);
	char* data = argv[3];
	int fd;
	char c;

	if ((fd = open(filename, O_WRONLY)) < 0){
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);
	}

	if (lseek(fd, offset, SEEK_SET) < 0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	write(fd, data, strlen(data));

	close(fd);

	exit(0);
}
