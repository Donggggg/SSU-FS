#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "person.h"

typedef struct _Node{
	int pagenum;
	int recordnum;
	struct _Node *next;
}Node;

Node *head;

void readPage(FILE *fp, char *pagebuf, int pagenum)
{
	fseek(fp, pagenum * PAGE_SIZE, SEEK_SET);
	fread(pagebuf, PAGE_SIZE, 1, fp);
}

void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	fseek(fp, pagenum * PAGE_SIZE, SEEK_SET);
	fwrite(pagebuf, PAGE_SIZE, 1, fp);
}

void pack(char *recordbuf, const Person *p)
{
	strcat(recordbuf, p->sn);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->name);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->age);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->addr);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->phone);
	strcat(recordbuf, "#");
	strcat(recordbuf, p->email);
	strcat(recordbuf, "#");
}

void unpack(const char *recordbuf, Person *p)
{

}

void insert(FILE *fp, const Person *p)
{
	int	isNextPage = PAGE_SIZE / RECORD_SIZE, offset;
	int total_pages, total_records, dpage_num, drecord_num;
	char *recordbuf = malloc(sizeof(char) * RECORD_SIZE);
	char *pagebuf = malloc(sizeof(char) * PAGE_SIZE);
	char *headerbuf = malloc(sizeof(char) * PAGE_SIZE);
	char *tmp = malloc(sizeof(char) * PAGE_SIZE);
	struct stat statbuf;

	pack(recordbuf, p);

	fstat(fp->_fileno, &statbuf);

	if(statbuf.st_size < 1) // 처음 쓰는 경우
	{
		// head 페이지 세팅 
		total_pages = 2;
		total_records = 0;
		dpage_num = -1;
		drecord_num = -1;

		memset(pagebuf, (char)0xff, PAGE_SIZE);
		memcpy(pagebuf, &total_pages, sizeof(int));
		memcpy(pagebuf+4, &total_records, sizeof(int));
		memcpy(pagebuf+8, &dpage_num, sizeof(int));
		memcpy(pagebuf+12, &drecord_num, sizeof(int));
		writePage(fp, pagebuf, 0);

		// 첫 번째 페이지 세팅
		memset(pagebuf, (char)0xff, PAGE_SIZE);
		writePage(fp, pagebuf, 1);
	}

	// head 페이지 정보 read
	readPage(fp, headerbuf, 0);
	memcpy(&total_pages, headerbuf, sizeof(int));
	memcpy(&total_records, headerbuf+4, sizeof(int));
	memcpy(&dpage_num, headerbuf+8, sizeof(int));
	memcpy(&drecord_num, headerbuf+12, sizeof(int));

	if(dpage_num == -1 && drecord_num == -1) // 삭제된 레코드가 없는 경우
	{
		if(total_records % isNextPage != 0 || total_records == 0) // 현재 페이지에 써야하는 경우
				total_pages--;
		else{ // 다음 페이지에 써야하는 경우
			memset(pagebuf, (char)0xff, PAGE_SIZE);
			writePage(fp, pagebuf, total_pages);
		}

		readPage(fp, pagebuf, total_pages);
		offset = (total_records%isNextPage) * RECORD_SIZE;
		memcpy(pagebuf + offset, recordbuf, RECORD_SIZE);
		writePage(fp, pagebuf, total_pages);

		total_pages++;
		total_records++;
		memcpy(headerbuf, &total_pages, sizeof(int));
		memcpy(headerbuf+4, &total_records, sizeof(int));
		writePage(fp,headerbuf, 0);
	}
	else // 삭제된 레코드가 있는 경우
	{
	}

}

void delete(FILE *fp, const char *sn)
{

}

Node* set_delete_list(FILE *fp)
{
	int record_num, page_num;
	struct stat statbuf;

	fstat(fp->_fileno, &statbuf);

	if(statbuf.st_size < 1) // 헤더 페이지 없는 경우
		return NULL;

	fseek(fp, 8, SEEK_SET);
//	fread((void*)page_num, 4, 1, fp);
//	fread((void*)record_num, 4, 1, fp);

	if(record_num == -1 && page_num == -1) // 삭제된 레코드가 없는 경우
		return NULL;

	fseek(fp, 0, SEEK_SET); // 오프셋 초기화
}

int main(int argc, char *argv[])
{
	FILE *fp;
	Person person; // Person 구조체 

	if((fp = fopen(argv[2], "r+")) < 0){ // data파일이 없는 경우
		fprintf(stderr, "fopen error for %s\n", argv[2]);
		exit(1);
	}

	//head = set_delete_list(fp);

	if(!strcmp(argv[1], "i"))
	{
		strcpy(person.sn, argv[3]);
		strcpy(person.name, argv[4]);
		strcpy(person.age, argv[5]);
		strcpy(person.addr, argv[6]);
		strcpy(person.phone, argv[7]);
		strcpy(person.email, argv[8]);

		insert(fp, &person);

	}
	else if(!strcmp(argv[1], "d"))
	{

	}
	else
		fprintf(stderr, "You can do only 'i' or 'd' at argv[1]\n");




	return 1;
}
