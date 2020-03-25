#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
	if (argc < 4){
		fprintf(stderr, "argv number error!\n");
		exit(1);
	}

	char* filename = argv[1];
	int offset = atoi(argv[2]);
	char* data = argv[3];
	int fd1, fd2;
	char c;

	if ((fd1 = open(filename, O_WRONLY)) < 0){
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);
	}

	if (lseek(fd1, offset, SEEK_SET) < 0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	if ((fd2 = open(data, O_RDONLY)) < 0){
		fprintf(stderr, "open error for %s\n", data);
		exit(1);
	}

	while(1){
		if (read(fd2, &c, 1) == 0)
			break;
		write(fd1, &c, 1);
	}

	close(fd1);
	close(fd2);

	exit(0);
}
