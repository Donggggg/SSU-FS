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
	int fd1, now;
	char c;
	char* temp_string;

	if ((fd1 = open(filename, O_RDWR)) < 0){
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);
	}

	if ((now = lseek(fd1, 0, SEEK_END)) < 0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	if (now > offset){
		temp_string = (char*)malloc(sizeof(char) * (now - offset));

		if (temp_string == NULL){
			fprintf(stderr, "malloc error\n");
			exit(1);
		}
	}

	if (lseek(fd1, offset, SEEK_SET) < 0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	if (now > offset)
	read(fd1, temp_string, now - offset);

	if (lseek(fd1, offset, SEEK_SET) < 0){
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	write(fd1, data, strlen(data));

	if (now > offset)
	write(fd1, temp_string, now - offset);

	close(fd1); 
	if ( now > offset)
	free(temp_string);
	exit(0);
}
