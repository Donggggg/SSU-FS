#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "person.h"

const int isNextPage = PAGE_SIZE / RECORD_SIZE;

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
	sprintf(recordbuf, "%s#%s#%s#%s#%s#%s#", p->sn, p->name, p->age, p->addr, p->phone, p->email);
}

void unpack(const char *recordbuf, Person *p)
{
	char temp_recordbuf[RECORD_SIZE];
	strcpy(temp_recordbuf, recordbuf);

	strcpy(p->sn, strtok(temp_recordbuf, "#"));
	strcpy(p->name, strtok(NULL, "#"));
	strcpy(p->age, strtok(NULL, "#"));
	strcpy(p->addr, strtok(NULL, "#"));
	strcpy(p->phone, strtok(NULL, "#"));
	strcpy(p->email, strtok(NULL, "#"));
}

void insert(FILE *fp, const Person *p)
{
	int offset, page_tmp, record_tmp;
	int total_pages, total_records, dpage_num, drecord_num;
	char *recordbuf = malloc(sizeof(char) * RECORD_SIZE);
	char *deletebuf = malloc(sizeof(char) * RECORD_SIZE);
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
		readPage(fp, pagebuf, dpage_num);
		memcpy(deletebuf, pagebuf+(drecord_num*RECORD_SIZE), RECORD_SIZE);

		memcpy(&page_tmp, deletebuf+1, sizeof(int)); // 다음 삭제 페이지 저장
		memcpy(&record_tmp, deletebuf+5, sizeof(int)); // 다음 삭제 페이지 저장

		memcpy(pagebuf+(drecord_num*RECORD_SIZE), recordbuf, RECORD_SIZE);
		writePage(fp, pagebuf, dpage_num); // 삭제 페이지에 레코드 저장 

		// haeder 페이지 갱신
		memcpy(headerbuf+8, &page_tmp, sizeof(int));
		memcpy(headerbuf+12, &record_tmp, sizeof(int));
		writePage(fp, headerbuf, 0);
	}
}

void delete(FILE *fp, const char *sn)
{
	int i, j, count = 0;
	int total_pages, total_records, dpage_num, drecord_num;
	char *tmp_sn = malloc(sizeof(char) * 14);
	char *recordbuf = malloc(sizeof(char) * RECORD_SIZE);
	char *pagebuf = malloc(sizeof(char) * PAGE_SIZE);
	char *headerbuf = malloc(sizeof(char) * PAGE_SIZE);
	char *tmp = malloc(sizeof(char) * PAGE_SIZE);
	Person person;

	readPage(fp, headerbuf, 0);
	memcpy(&total_pages, headerbuf, sizeof(int));
	memcpy(&total_records, headerbuf+4, sizeof(int));
	memcpy(&dpage_num, headerbuf+8, sizeof(int));
	memcpy(&drecord_num, headerbuf+12, sizeof(int));

	for(i = 1; i < total_pages; i++){
		readPage(fp, pagebuf, i);

		while(count != isNextPage){
			memcpy(recordbuf, pagebuf + (count*RECORD_SIZE), RECORD_SIZE);

			if(recordbuf[0]=='*'){ // 삭제된 데이터면 검사 스킵
				count++;
				continue;
			}
			else // recordbuf unpack해서 person에 저장 
				unpack(recordbuf, &person);

			if(!strcmp(person.sn, sn)){ // 주민 번호 일치하는 경우 삭제 진행
				memset(pagebuf + (count*RECORD_SIZE), (char)0xff, RECORD_SIZE);
				memset(pagebuf + (count*RECORD_SIZE), '*', 1);
				memcpy(pagebuf + (count*RECORD_SIZE) + 1, &dpage_num, sizeof(int));
				memcpy(pagebuf + (count*RECORD_SIZE) + 5, &drecord_num, sizeof(int));
				writePage(fp, pagebuf, i);

				// header 페이지 갱신
				memcpy(headerbuf+8, &i, sizeof(int));
				memcpy(headerbuf+12, &count, sizeof(int));
				writePage(fp, headerbuf, 0);

				return ;
			}
			count++;
		}
		count = 0;
	}
}

int main(int argc, char *argv[])
{
	FILE *fp;
	Person person; // Person 구조체 

	if(argc < 2){
		fprintf(stderr, "more argv please\n");
		exit(1);
	}

	if((fp = fopen(argv[2], "r+")) < 0){ // data파일이 없는 경우
		fprintf(stderr, "fopen error for %s\n", argv[2]);
		exit(1);
	}

	if(!strcmp(argv[1], "i"))
	{
		if(argc < 9){
			fprintf(stderr, "more argv please\n");
			exit(1);
		}

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
		if(argc < 4){
			fprintf(stderr, "more argv please\n");
			exit(1);
		}

		delete(fp, argv[3]);
	}
	else
		fprintf(stderr, "You can do only 'i' or 'd' at argv[1]\n");

	fclose(fp);
	return 1;
}
