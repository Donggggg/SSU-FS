#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "person.h"

// 페이지 번호에 해당하는 페이지를 주어진 페이지 버퍼에 읽어서 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void readPage(FILE *fp, char *pagebuf, int pagenum)
{
	fseek(fp, pagenum*PAGE_SIZE, SEEK_SET);
	fread(pagebuf, PAGE_SIZE, 1, fp);
}

// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 위치에 저장한다. 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//
void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	fseek(fp, pagenum*PAGE_SIZE, SEEK_SET);
	fwrite(pagebuf, PAGE_SIZE, 1, fp);
}

// 주어진 레코드 파일에서 레코드를 읽어 heap을 만들어 나간다. Heap은 배열을 이용하여 저장되며, 
// heap의 생성은 Chap9에서 제시한 알고리즘을 따른다. 레코드를 읽을 때 페이지 단위를 사용한다는 것에 주의해야 한다.
void buildHeap(FILE *inputfp, char **heaparray)
{
	int i;
	int cur_page = 1, cur_record = 0;
	int pagenum, recordnum;
	char header_page[PAGE_SIZE], page[PAGE_SIZE];
	char **tmparray;

	readPage(inputfp, header_page, 0);
	memcpy(&pagenum, head_page, sizeof(int));
	memcpy(&recordnum, head_page+4, sizeof(int));

	heaparray = malloc(sizeof(char*) * recordnum);
	tmparray = malloc(sizeof(char*) * recordnum);
	for(i = 0; i < pagenum; i++){
		heaparray[i] = malloc(sizeof(char) * RECORD_SIZE);
		tmparray[i] = malloc(sizeof(char) * RECORD_SIZE);
	}

	while(cur_page < pagenum)
	{
		readPage(inputfp, page, cur_page);

		for(i = 0; i < PAGE_SIZE / RECORD_SIZE; i++)
			strncpy(tmparray[cur_record++], page+(i*RECORD_SIZE), sizeof(Person));

		cur_page++;
	}

	makeHeap(tmparray, heaparray, cur_record - 1);
}

//
// 완성한 heap을 이용하여 주민번호를 기준으로 오름차순으로 레코드를 정렬하여 새로운 레코드 파일에 저장한다.
// Heap을 이용한 정렬은 Chap9에서 제시한 알고리즘을 이용한다.
// 레코드를 순서대로 저장할 때도 페이지 단위를 사용한다.
//
void makeSortedFile(FILE *outputfp, char **heaparray)
{


}

void makeHeap(char **tmparray, char **heaparray, int count)
{
	int heapnum = count, cur = 0, prt;
	char tmp[RECORD_SIZE];

	while(cur != 0){
		strcpy(heaparray[cur], tmparray[heapnum]);

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

}

int main(int argc, char *argv[])
{
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

	buildHeap(inputfp, heaparray);

	return 1;
}
