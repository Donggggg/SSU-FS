#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "flash.h"

int dd_read(int ppn, char *pagebuf); 
int dd_write(int ppn, char *pagebuf);
int dd_erase(int pbn);

FILE *flashfp;	// fdevicedriver.c에서 사용

int main(int argc, char *argv[])
{	
	char sectorbuf[SECTOR_SIZE];
	char sparebuf[SPARE_SIZE];
	char pagebuf[PAGE_SIZE];
	char blockbuf[BLOCK_SIZE];
	char *buf;
	char c;
	char option = argv[1][0];
	char print_page[PAGE_SIZE];
	char sector_chk, spare_chk;
	int block_num, page_num, free_page_num;
	int size;
	int i, j;

	switch(option)
	{
		case 'c' : // c옵션
			memset((void*)blockbuf, (char)0xFF, BLOCK_SIZE);
			flashfp = fopen(argv[2], "w");
			block_num = atoi(argv[3]);

			for(i = 0; i < block_num; i++)
				fwrite((void*)blockbuf, BLOCK_SIZE, 1, flashfp);

			break;
		case 'w' : // w옵션
			if(strlen(argv[4]) > SECTOR_SIZE || strlen(argv[5]) > SPARE_SIZE){
				fprintf(stderr, "size of data error\n");
				exit(1);
			}

			flashfp = fopen(argv[2], "r+");
			page_num = atoi(argv[3]);
			memcpy(sectorbuf, argv[4], strlen(argv[4]));
			memcpy(sparebuf, argv[5], strlen(argv[5]));

			fseek(flashfp, 0, SEEK_END);
			size = ftell(flashfp) / BLOCK_SIZE; // 총 block 개수 

			if(size * PAGE_NUM <= page_num){
				fprintf(stderr, "page number is wrong!\n");
				exit(1);
			}

			fseek(flashfp, page_num * PAGE_SIZE , SEEK_SET); // 해당 페이지 데이터 체크
			sector_chk = fgetc(flashfp);
			fseek(flashfp, SECTOR_SIZE - 1, SEEK_CUR);
			spare_chk = fgetc(flashfp);

			if(sector_chk == (char)0xFF && spare_chk == (char)0xFF){ // 페이지가 비어있는 경우
				memset(pagebuf, (char)0xFF, PAGE_SIZE);
				memcpy(pagebuf, sectorbuf, strlen(sectorbuf));
				memcpy(pagebuf + SECTOR_SIZE, sparebuf, strlen(sparebuf));

				if(dd_write(page_num, pagebuf) < 0){
					fprintf(stderr, "dd_write fucntion error\n");
					exit(1);
				}
			}
			else { // 페이지가 차있는 경우 
				while(1){ // free block 선정
					block_num = rand() % size;

					if(block_num  == page_num / 4)
						block_num = rand() % size;
						
					fseek(flashfp, block_num * BLOCK_SIZE, SEEK_SET);

					for(i = 0; i < PAGE_NUM; i++){
						if((c = fgetc(flashfp)) != (char)0xFF){
							break;
						}
						fseek(flashfp, SECTOR_SIZE - 1, SEEK_CUR);
						if((c = fgetc(flashfp)) != (char)0xFF){
							break;
						}
						fseek(flashfp, SPARE_SIZE - 1, SEEK_CUR);
					}

					if(i == PAGE_NUM)
						break;
				}
				
				for(i = 1; i < PAGE_NUM; i++){ // free block으로 복사
					if(++page_num % PAGE_NUM == 0)
						page_num = page_num - PAGE_NUM;
					dd_read(page_num, pagebuf);
					free_page_num = block_num * PAGE_NUM + (page_num % PAGE_NUM);
					dd_write(free_page_num, pagebuf);
				}

				dd_erase(page_num / PAGE_NUM); // write할 block 삭제 

				for(i = 0; i < PAGE_NUM; i++){ // write할 block으로 원래 데이터 이동
					++free_page_num;
					++page_num;
					if(free_page_num % PAGE_NUM == 0 && page_num % PAGE_NUM == 0){
						free_page_num = free_page_num - PAGE_NUM;
						page_num = page_num - PAGE_NUM;
					}
					dd_read(free_page_num, pagebuf);
					dd_write(page_num, pagebuf);
				}

				if(++page_num % PAGE_NUM == 0) // 페이지 다운
					page_num -= PAGE_NUM;

				dd_erase(block_num); // free block 삭제 

				memset(pagebuf, (char)0xFF, PAGE_SIZE);
				memcpy(pagebuf, sectorbuf, strlen(sectorbuf));
				memcpy(pagebuf + SECTOR_SIZE, sparebuf, strlen(sparebuf));

				if(dd_write(page_num, pagebuf) < 0){
					fprintf(stderr, "dd_write fucntion error\n");
					exit(1);
				}
			}
			break;
		case 'r' :
			flashfp = fopen(argv[2], "r");
			page_num = atoi(argv[3]);

			if(dd_read(page_num, pagebuf) < 0){
				fprintf(stderr, "dd_read function error\n");
				exit(1);
			}

			i = 0;
			j = 0;

			while(pagebuf[i] != (char)0xFF)
				print_page[j++] = pagebuf[i++];

			i = 512;

			while(pagebuf[i] != (char)0xFF){
				if(i==512)
					print_page[j++] = ' '; // spare에 data가 있으면
				print_page[j++] = pagebuf[i++];
			}

			print_page[j] = '\0'; // 마지막 명시

			if(j > 0)
				printf("%s", print_page);

			break;
		case 'e' :
			flashfp = fopen(argv[2], "r+");
			block_num = atoi(argv[3]);

			if(dd_erase(block_num) < 0){
				fprintf(stderr, "dd_erase function error\n");
				exit(1);
			}

			break;
		default : 
			fprintf(stderr, "option is wrong\nonly c, w, r, e\n");
			exit(1);
	}

	fclose(flashfp);
	return 0;
}
