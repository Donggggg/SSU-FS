#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[])
{
	if (argc < 4){
		fprintf(stderr, "arvc number wrong!\n");
		exit(1);
	}

	char* filename = argv[1];
	int offset = atoi(argv[2]);
	int bytes = atoi(argv[3]);
	int fd, size;
	char* head;
	char* tail;

	if ((fd = open(filename, O_RDONLY)) < 0){
		fprintf(stderr, "error for open %s\n", filename);
		exit(1);
	}

	if ((size = lseek(fd, 0, SEEK_END)) < 0){
		fprintf(stderr, "lseek error1\n");
		exit(1);
	}
	printf("size : %d off : %d bytes : %d\n", size, offset, bytes);

	if (lseek(fd, 0, SEEK_SET) < 0){
		fprintf(stderr, "lseek error2\n");
		exit(1);
	}

	head = (char*)malloc(sizeof(char) * offset);
	read(fd, head, offset);

	if (lseek(fd, bytes, SEEK_CUR) < 0){
		fprintf(stderr, "lseek error3\n");
		exit(1);
	}

	tail = (char*)malloc(sizeof(char) * (size - offset - bytes));
	read(fd, tail, size - offset - bytes);
	printf("tail  : %s\n", tail);
	printf("size : %d off : %d bytes : %d\n", size, offset, bytes);

	close(fd);

	if ((fd = open(filename, O_WRONLY | O_TRUNC)) < 0){
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);
	}

	write(fd, head, offset);
	write(fd, tail, size - offset - bytes);

	close(fd);
	free(head);
	free(tail);

	exit(0);
}
