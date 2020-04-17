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
	char option = argv[1][0];
	char print_page[PAGE_SIZE];
	int block_num, page_num;
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
			flashfp = fopen(argv[2], "r+");
			page_num = atoi(argv[3]);

			if(strlen(argv[4]) > SECTOR_SIZE || strlen(argv[5]) > SPARE_SIZE){
				fprintf(stderr, "size of data error\n");
				exit(1);
			}

			memcpy(sectorbuf, argv[4], strlen(argv[4]));
			memcpy(sparebuf, argv[5], strlen(argv[5]));

			memset(pagebuf, (char)0xFF, PAGE_SIZE);

			memcpy(pagebuf, sectorbuf, strlen(sectorbuf));
			memcpy(pagebuf + 512, sparebuf, strlen(sparebuf));

			if(dd_write(page_num, pagebuf) < 0){
				fprintf(stderr, "dd_write fucntion error\n");
				exit(1);
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
	}

	fclose(flashfp);
	return 0;
}
