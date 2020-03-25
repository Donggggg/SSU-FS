#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
	if (argc != 3){
		fprintf(stderr, "argc number error!\n");
		exit(1);
	}

	char* of = argv[1];
	char* cf = argv[2];
	char string[100];
	int fd1, fd2;
	int length;

	if ((fd1 = open(argv[1], O_RDONLY)) < 0){
		fprintf(stderr, "open error for %s\n", argv[1]);
		exit(1);
	}

	if ((fd2 = open(argv[2], O_CREAT | O_WRONLY, 0400)) < 0){
		fprintf(stderr, "open error for %s\n", argv[2]);
		exit(1);
	}

	while ((length = read(fd1, &string, 100)) > 0){
		write(fd2, &string, length);
	}

	close(fd1);
	close(fd2);

	exit(0);
}
