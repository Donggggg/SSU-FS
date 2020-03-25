#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
	if (argc != 3){
		fprintf(stderr, "argc number is wrong!\n");
		exit(1);
	}

	char* ori_file = argv[1];
	char* co_file = argv[2];
	int fd1, fd2;
	char c;

	if ((fd1 = open(ori_file, O_WRONLY | O_APPEND)) < 0){
		fprintf(stderr, "error for open %s\n", ori_file);
		exit(1);
	}

	if ((fd2 = open(co_file, O_RDONLY)) < 0){
		fprintf(stderr, "error for open %s\n", co_file);
		exit(1);
	}

	while (1){
		if (read(fd2, &c, 1) == 0)
			break;
		write(fd1, &c, 1);
	}

	close(fd1);
	close(fd2);
	
	exit(0);
}
