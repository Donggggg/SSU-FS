#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct _Student{
	char name[31];
	char adderess[71];
}Student;

int main(int argc, char **argv){
	int i;
	struct stat file_info;
	struct timeval start,end;
	int times1,times2;
	FILE *fp = fopen(argv[1],"r");
	stat(argv[1],&file_info);
	int num = file_info.st_size/100;

	Student buffer;

	gettimeofday(&start,NULL);
	for(i = 0; i < num; i++){
		fread(&buffer,100,1,fp);
	}
	gettimeofday(&end,NULL);
	times2 = (end.tv_sec-start.tv_sec)*1000000;
	times1 = end.tv_usec-start.tv_usec;
	printf("# records : %d timecost : %d us\n",num,times1+times2);
	fclose(fp);
	return 0;
}

