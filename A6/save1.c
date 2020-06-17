#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "person.h"

int pagenum, recordnum;
char **tmparray;

void readPage(FILE *fp, char *pagebuf, int pagenum)
{
	fseek(fp, pagenum*PAGE_SIZE, SEEK_SET);
	fread(pagebuf, PAGE_SIZE, 1, fp);
}

void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	fseek(fp, pagenum*PAGE_SIZE, SEEK_SET);
	fwrite(pagebuf, PAGE_SIZE, 1, fp);
}

void buildHeap(FILE *inputfp, char **heaparray)
{
	int i;
	int cur_page = 1, cur_record = 0;
	int heapnum, cur, prt;
	char tmp[RECORD_SIZE];
	char header_page[PAGE_SIZE], page[PAGE_SIZE];

	while(cur_page < pagenum)
	{
		readPage(inputfp, page, cur_page);

		for(i = 0; i < PAGE_SIZE / RECORD_SIZE; i++){

			if(cur_record >= recordnum)
				break;

			strncpy(tmparray[cur_record++], page+(i*RECORD_SIZE), sizeof(Person));
		}
		cur_page++;
	}

	heapnum = cur_record - 1;
	cur = 0;

	while(heapnum >= 0){
		strcpy(heaparray[cur], tmparray[heapnum]);

		if(cur == 0){
			cur++;
			heapnum--;
			continue;
		}

		if(cur % 2 == 0)
			prt = cur / 2 - 1;
		else
			prt = cur / 2;

		if(atoi(heaparray[cur]) < atoi(heaparray[prt])){
			strcpy(tmp, heaparray[cur]);
			strcpy(heaparray[cur], heaparray[prt]);
			strcpy(heaparray[prt], tmp);
		}

		cur++;
		heapnum--;
	}

//	for(i = 0 ; i < recordnum; i++)
//		printf(">%s\n", heaparray[i]);

}

void makeSortedFile(FILE *outputfp, char **heaparray)
{
	int i, cur_page = 0, cur_record = 0;
	int last = recordnum-1, chd1, chd2, cur;
	char tmp[RECORD_SIZE];
	char pagebuf[PAGE_SIZE];

	memset(pagebuf, (char)0xff, PAGE_SIZE);
	memcpy(pagebuf, &pagenum, sizeof(int));
	memcpy(pagebuf+4, &recordnum, sizeof(int));
	writePage(outputfp, pagebuf, cur_page++);

	while(pagenum > cur_page){
		memset(pagebuf, (char)0xff, PAGE_SIZE);

		for(i = 0; i < PAGE_SIZE/RECORD_SIZE; i++){
			if(cur_record >= recordnum)
				break;

			memcpy(pagebuf+(i*RECORD_SIZE), heaparray[0], RECORD_SIZE);
			printf("push[%d] : %s\n", cur_record,heaparray[0]);

			cur = 0;
			printf("l : %d\n", last);
			printf("lastnode :%s\n", heaparray[last]);
			strcpy(heaparray[cur], heaparray[last--]);
	//		free(heaparray[last--]);
			printf("up : %s\n", heaparray[0]);

			while(1){
				if(cur == 0){
					chd1 = 1;
					chd2 = 2;
				}
				else{
					chd1 = 2 * cur + 1;
					chd2 = 2 * cur + 2;
				}
				printf("cur :%d chd1 : %d chd2: %d\n", cur, chd1, chd2);

				if(heaparray[chd1] == NULL && heaparray[chd2] == NULL)
					break;

				if(atoi(heaparray[chd1]) > atoi(heaparray[chd2]))
					chd1 = chd2;

				//printf("%s// %s\n", heaparray[cur], heaparray[chd1]);

				if(atoi(heaparray[chd1]) > atoi(heaparray[cur]))
					break;
				else{
					strcpy(tmp, heaparray[cur]);
					strcpy(heaparray[cur], heaparray[chd1]);
					strcpy(heaparray[chd1], tmp);
				}
				cur = chd1;
			}
			for(int a = 0; a < last+1; a++)
				printf("%s\n", heaparray[a]);
			cur_record++;
		}
		printf("next page\n\n");
		writePage(outputfp, pagebuf, cur_page);

		cur_page++;
	}
}

int main(int argc, char *argv[])
{
	int i;
	char header_page[PAGE_SIZE], page[PAGE_SIZE];
	char **heaparray;
	FILE *inputfp;	// 입력 레코드 파일의 파일 포인터
	FILE *outputfp;	// 정렬된 레코드 파일의 파일 포인터

	if(argc != 4){ // 입력 개수 오류 처리 
		fprintf(stderr, "args error\n");
		exit(1);
	}

	if(strcmp(argv[1], "r")){ // 없는 옵션 오류 처리
		fprintf(stderr, "only r option\n");
		exit(1);
	}

	inputfp = fopen(argv[2], "r");
	outputfp = fopen(argv[3], "w+");

	if(inputfp == NULL || outputfp == NULL){ // fopen error
		fprintf(stderr, "fopen error\n");
		exit(1);
	}

	readPage(inputfp, header_page, 0);
	memcpy(&pagenum, header_page, sizeof(int));
	memcpy(&recordnum, header_page+4, sizeof(int));

	heaparray = malloc(sizeof(char*) * recordnum);
	tmparray = malloc(sizeof(char*) * recordnum);
	for(i = 0; i < recordnum; i++){
		heaparray[i] = malloc(sizeof(char) * RECORD_SIZE);
		tmparray[i] = malloc(sizeof(char) * RECORD_SIZE);
	}

	buildHeap(inputfp, heaparray);

	makeSortedFile(outputfp, heaparray);

	return 1;
}
