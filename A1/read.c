#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
	if (argc != 4){
		fprintf(stderr, "argc number error!\n");
		exit(1);
	}

	char* filename = argv[1];
	int offset = atoi(argv[2]);
	int bytes = atoi(argv[3]);
	int fd;
	char c;
	
	if ((fd = open(filename, O_RDONLY)) < 0){
			fprintf(stderr, "error for open %s\n", argv[1]);
			exit(1);
	}

	if (lseek(fd, offset, SEEK_SET) < 0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	while (bytes--){
		if (read(fd, &c, 1)==0)
			break;
		printf("%c", c);
			}

	close(fd);
	exit(0);
}
