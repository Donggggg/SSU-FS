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
	int fd1, fd2, now;
	char c;

	if ((fd1 = open(filename, O_RDWR)) < 0){
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);
	}

	if ((now = lseek(fd1, 0, SEEK_END)) < 0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	char temp_string[now-offset];

	if (temp_string == NULL){
		fprintf(stderr, "malloc error\n");
		exit(1);
	}

	if (lseek(fd1, offset, SEEK_SET) < 0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	read(fd1, &temp_string, now - offset);
	printf("string : %ld now : %d offset : %d \n", strlen(temp_string),now-offset, now);

	if ((fd2 = open(data, O_RDONLY)) < 0){
		fprintf(stderr, "open error for %s\n", data);
		exit(1);
	}

	if (lseek(fd1, offset, SEEK_SET) < 0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	while(1){
		if (read(fd2, &c, 1) == 0)
			break;
		write(fd1, &c, 1);
	}

	close(fd2);

	if (lseek(fd1, 0, SEEK_END) < 0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	write(fd1, &temp_string, now - offset);

	close(fd1); 
	exit(0);
}
