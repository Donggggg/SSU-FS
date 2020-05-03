
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "sectormap.h"

int dd_read(int ppn, char *pagebuf);
int dd_write(int lsn, char *pagebuf);
int dd_erase(int pbn);

int table[DATAPAGES_PER_DEVICE][2];
int isExist[DATAPAGES_PER_DEVICE];
SpareData spare_data_list[DATAPAGES_PER_DEVICE];
FILE *flashfp;

void ftl_open()
{
	int i;

	for(i = 0; i < DATAPAGES_PER_DEVICE; i++){
		table[i][0] = i;
		table[i][1] = -1;
		isExist[i] = FALSE;
		spare_data_list[i].lpn = -1;
		spare_data_list[i].is_invalid = TRUE;
		memset(spare_data_list[i].dummy, (char)0xFF, SPARE_SIZE - 8);
	}
	return;
}

void ftl_read(int lsn, char *sectorbuf)
{
	int ppn;
	char *pagebuf = malloc(sizeof(char) * PAGE_SIZE);

	if(lsn >= DATAPAGES_PER_DEVICE || lsn < 0){
		fprintf(stderr, "lsn is out of range\n");
		exit(1);
	}

	if((ppn = table[lsn][1]) < 0){
		printf("lsn %d has not ppn num, check your table!\n", lsn);
		return ;
	}

	dd_read(ppn, pagebuf);

	memcpy(sectorbuf, pagebuf, SECTOR_SIZE);

	return;
}

void ftl_write(int lsn, char *sectorbuf)
{
	int i,j, max = 0, gp_count[DATABLKS_PER_DEVICE];
	int gb = 0, gp, fb = DATABLKS_PER_DEVICE;
	int check, isThereGarbage = FALSE;
	char *str = malloc(sizeof(char) * 4);
	char *pagebuf = malloc(sizeof(char) * PAGE_SIZE);

	if(lsn >= DATAPAGES_PER_DEVICE || lsn < 0){
		fprintf(stderr, "lsn is out of range\n");
		exit(1);
	}

	for(i = 0; i < DATAPAGES_PER_DEVICE; i++){ // free page를 검사
		if(spare_data_list[i].is_invalid == FALSE) // garbage페이지가 있으면 체크 
			isThereGarbage = TRUE;
		if(spare_data_list[i].is_invalid == TRUE && isExist[i] == FALSE) // 빈 페이지 있으면 break
			break;
	}

	if(i == DATAPAGES_PER_DEVICE){ // free page가 없는 경우 
		if(isThereGarbage = FALSE){ // garbage page가 없는 경우
			gb = table[lsn][1] / PAGES_PER_BLOCK;
			gp = gb * PAGES_PER_BLOCK;

			for(i = gp, j = 0; i < gp + PAGES_PER_BLOCK; i++, j++){
				if(i == table[lsn][1]){ // page가 lsn에 대응하는 ppn이면
					isExist[i] = FALSE;
					spare_data_list[i].lpn =  -1;
					spare_data_list[i].is_invalid = TRUE;
				}
				else if(spare_data_list[i].is_invalid == TRUE){ // 그 외의 경우
					fseek(flashfp, PAGE_SIZE * i, SEEK_SET);
					fread(pagebuf, PAGE_SIZE, 1, flashfp); // 가비지 블록의 페이지 read
					fseek(flashfp, PAGE_SIZE * ((fb*PAGES_PER_BLOCK)+j), SEEK_SET);
					fwrite(pagebuf, PAGE_SIZE, 1, flashfp); // 프리 블록에 페이지 write
					fflush(flashfp);
				}
			}

			dd_erase(gb); // garbage 블럭 정리

			for(i = gp, j = 0; i < gp + PAGES_PER_BLOCK; i++, j++){
				fseek(flashfp, PAGE_SIZE * ((fb*PAGES_PER_BLOCK)+j), SEEK_SET);
				fread(pagebuf, PAGE_SIZE, 1, flashfp); // 프리 블록 page read
				fseek(flashfp, PAGE_SIZE * i, SEEK_SET);
				fwrite(pagebuf, PAGE_SIZE, 1, flashfp); // 초기화된 블록으로 write
				fflush(flashfp);
			} 

			dd_erase(DATABLKS_PER_DEVICE); // free block 정리 

		}
		else{ // garbage page가 1개 이상인 경우
			for(i = 0; i < DATABLKS_PER_DEVICE; i++)
				gp_count[i] = 0;

			for(i = 0; i < DATAPAGES_PER_DEVICE; i++) // block마다 garbage page개수 측정
				if(spare_data_list[i].is_invalid == FALSE)
					gp_count[i/PAGES_PER_BLOCK]++;

			for(i = 0; i < DATABLKS_PER_DEVICE; i++) // garbage block 선정
				if(gp_count[i] > max){
					gb = i;
					max = gp_count[i];
				}

			gp = gb*PAGES_PER_BLOCK;

			for(i = gp, j = 0; i < gp + PAGES_PER_BLOCK; i++, j++){
				if(spare_data_list[i].is_invalid == TRUE){ // page에 유효 데이터있으면
					fseek(flashfp, PAGE_SIZE * i, SEEK_SET);
					fread(pagebuf, PAGE_SIZE, 1, flashfp); // 가비지 블록의 페이지 read
					fseek(flashfp, PAGE_SIZE * ((fb*PAGES_PER_BLOCK)+j), SEEK_SET);
					fwrite(pagebuf, PAGE_SIZE, 1, flashfp); // 프리 블록에 페이지 write
					fflush(flashfp);
				}
				else if(spare_data_list[i].is_invalid == FALSE){ // page가 garbage이면
					isExist[i] = FALSE;
					spare_data_list[i].lpn =  -1;
					spare_data_list[i].is_invalid = TRUE;
				}
			}

			dd_erase(gb); // garbage 블럭 정리

			for(i = gp, j = 0; i < gp + PAGES_PER_BLOCK; i++, j++){
				fseek(flashfp, PAGE_SIZE * ((fb*PAGES_PER_BLOCK)+j), SEEK_SET);
				fread(pagebuf, PAGE_SIZE, 1, flashfp); // 프리 블록 page read
				fseek(flashfp, PAGE_SIZE * i, SEEK_SET);
				fwrite(pagebuf, PAGE_SIZE, 1, flashfp); // 초기화된 블록으로 write
				fflush(flashfp);
			} 

			dd_erase(DATABLKS_PER_DEVICE); // free block 정리 
		}
	}

	if(table[lsn][1] > -1) // 업데이트 필요할 경우
	{
		for(i = 0; i < DATAPAGES_PER_DEVICE; i++) //free page 선정
		{ 
			check = TRUE;

			for(j = 0; j < DATAPAGES_PER_DEVICE; j++) // table에 존재하는 page인지
				if(table[j][1] == i){
					check = FALSE;
					break;
				}

			if(spare_data_list[i].is_invalid == FALSE) // page가 가비지인지
				continue; 

			if(check){
				isExist[i] = TRUE; // ppn
				spare_data_list[i].lpn = lsn; // ppn
				spare_data_list[table[lsn][1]].is_invalid = FALSE;
				table[lsn][1] = i;
				lsn = i;
				break;
			}
			else
				continue;
		}
	} 
	else // 업데이트 필요 없는 경우
	{
		for(i = 0; i < DATAPAGES_PER_DEVICE; i++) //free page 선정
		{ 
			check = TRUE;

			for(j = 0; j < DATAPAGES_PER_DEVICE; j++) // table에 존재하는 page인지
				if(table[j][1] == i){
					check = FALSE;
					break;
				}

			if(spare_data_list[i].is_invalid == FALSE) // page가 가비지인지
				continue;

			if(check){ //page 선정이 되었으면
				spare_data_list[i].lpn = lsn;
				table[lsn][1] = i;
				isExist[i] = TRUE;
				lsn = i;
				break;
			}
			else // 안되었으면 다시 확인 
				continue;
		}
	}

	memset(pagebuf, (char)0xFF, PAGE_SIZE);
	memcpy(pagebuf, sectorbuf, strlen(sectorbuf));
	memcpy(pagebuf + SECTOR_SIZE, &spare_data_list[lsn].lpn, sizeof(int));

	dd_write(lsn, pagebuf); 
	fflush(flashfp);

	return;
}

void ftl_print()
{
	int i;

	printf("lpn ppn\n");

	for(i = 0; i < DATAPAGES_PER_DEVICE; i++)
		printf("%d   %d\n", table[i][0], table[i][1]);

	printf("free block's pbn=%d\n", DATABLKS_PER_DEVICE);

	return;
}
