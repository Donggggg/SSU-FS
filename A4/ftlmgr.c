#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "sectormap.h"

int dd_read(int ppn, char *pagebuf);
int dd_write(int lsn, char *pagebuf);
int dd_erase(int pbn);

int table[DATAPAGES_PER_DEVICE + PAGES_PER_BLOCK][2];
int isExist[DATAPAGES_PER_DEVICE + PAGES_PER_BLOCK];
int free_block;
SpareData spare_data_list[DATAPAGES_PER_DEVICE + PAGES_PER_BLOCK];
FILE *flashfp;

void ftl_open()
{
	int i;
	free_block = DATABLKS_PER_DEVICE;

	for(i = 0; i < DATAPAGES_PER_DEVICE + 4; i++){
		table[i][0] = i;
		table[i][1] = -1;
		isExist[i] = FALSE;
		spare_data_list[i].lpn = -1;
		memset(spare_data_list[i].dummy, (char)0xFF, SPARE_SIZE - 8);
		spare_data_list[i].is_invalid = TRUE;

		if(i >= DATAPAGES_PER_DEVICE)
			spare_data_list[i].is_invalid = FALSE;
	}
	table[DATAPAGES_PER_DEVICE + PAGES_PER_BLOCK][1] = -1;

	return;
}

void ftl_read(int lsn, char *sectorbuf)
{
	int ppn;
	char *pagebuf = malloc(sizeof(char) * PAGE_SIZE);

	if(lsn >= DATAPAGES_PER_DEVICE + PAGES_PER_BLOCK || lsn < 0){
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
	int i,j, max = 0, gp_count[DATABLKS_PER_DEVICE+1];
	int gb = 0, gp;
	int check, isThereGarbage = FALSE;
	char *str = malloc(sizeof(char) * 4);
	char *pagebuf = malloc(sizeof(char) * PAGE_SIZE);

	if(lsn >= DATAPAGES_PER_DEVICE + PAGES_PER_BLOCK || lsn < 0){
		fprintf(stderr, "lsn is out of range\n");
		exit(1);
	}

	for(i = 0; i < DATAPAGES_PER_DEVICE + PAGES_PER_BLOCK; i++){ // free page를 검사
		if(i >= free_block * 4 && i < (free_block + 1) * 4) // freeblock의 시작페이지면 스킵
			continue;

		if(spare_data_list[i].is_invalid == FALSE){ // garbage페이지가 있으면 체크 
			isThereGarbage = TRUE;
		}

		if(spare_data_list[i].is_invalid == TRUE && isExist[i] == FALSE) // 빈 페이지 있으면 break
			break;
	}

	if(i == DATAPAGES_PER_DEVICE + PAGES_PER_BLOCK){ // free page가 없는 경우 
		if(isThereGarbage == FALSE){ // garbage page가 없는 경우
			gb = table[lsn][1] / PAGES_PER_BLOCK;
			gp = gb * PAGES_PER_BLOCK;

			for(i = gp, j = 0; i < gp + PAGES_PER_BLOCK; i++, j++){
				if(i == table[lsn][1]){ // page가 lsn에 대응하는 ppn이면
					spare_data_list[(free_block*4)+j].lpn = -1; //free page에 정보 이전
					spare_data_list[(free_block*4)+j].is_invalid = TRUE; 
					isExist[(free_block*4)+j] = FALSE;

					spare_data_list[i].lpn =  -1;
					spare_data_list[i].is_invalid = FALSE;
					isExist[i] = FALSE;

					table[lsn][1] = -1;
				}
				else if(spare_data_list[i].is_invalid == TRUE){ // 그 외의 경우
					fseek(flashfp, PAGE_SIZE * i, SEEK_SET);
					fread(pagebuf, PAGE_SIZE, 1, flashfp); // 가비지 블록의 페이지 read
					fseek(flashfp, PAGE_SIZE * ((free_block*PAGES_PER_BLOCK)+j), SEEK_SET);
					fwrite(pagebuf, PAGE_SIZE, 1, flashfp); // 프리 블록에 페이지 write
					fflush(flashfp);

					spare_data_list[(free_block*4)+j] = spare_data_list[i]; //free page에 정보 이전
					isExist[(free_block*4)+j] = isExist[i];

					spare_data_list[i].lpn = -1; // free block화
					spare_data_list[i].is_invalid = FALSE;
					isExist[i] = FALSE;
				}
			}

			dd_erase(gb); // garbage 블럭 정리

		}
		else{ // garbage page가 1개 이상인 경우
			for(i = 0; i < DATABLKS_PER_DEVICE + 1; i++)
				gp_count[i] = 0;

			for(i = 0; i < DATAPAGES_PER_DEVICE + 4; i++){ // block마다 garbage page개수 측정
				if(spare_data_list[i].is_invalid == FALSE && isExist[i] == FALSE) // freepage면 continue
					continue;
				if(spare_data_list[i].is_invalid == FALSE)
					gp_count[i/PAGES_PER_BLOCK]++;
			}

			for(i = 0; i < DATABLKS_PER_DEVICE + 1; i++) // garbage block 선정
				if(gp_count[i] > max){
					gb = i;
					max = gp_count[i];
				}

			gp = gb*PAGES_PER_BLOCK;

			for(i = gp, j = 0; i < gp + PAGES_PER_BLOCK; i++, j++){
				if(spare_data_list[i].is_invalid == TRUE){ // page에 유효 데이터있으면
					fseek(flashfp, PAGE_SIZE * i, SEEK_SET);
					fread(pagebuf, PAGE_SIZE, 1, flashfp); // 가비지 블록의 페이지 read
					fseek(flashfp, PAGE_SIZE * ((free_block*PAGES_PER_BLOCK)+j), SEEK_SET);
					fwrite(pagebuf, PAGE_SIZE, 1, flashfp); // 프리 블록에 페이지 write
					fflush(flashfp);

					spare_data_list[(free_block*4)+j] = spare_data_list[i]; //free page에 정보 이전
					isExist[(free_block*4)+j] = isExist[i];

					spare_data_list[i].lpn = -1; // free block화
					spare_data_list[i].is_invalid = FALSE;
					isExist[i] = FALSE;
				}
				else if(spare_data_list[i].is_invalid == FALSE){ // page가 garbage이면
					spare_data_list[(free_block*4)+j].lpn = -1; //free page에 정보 이전
					spare_data_list[(free_block*4)+j].is_invalid = TRUE; 
					isExist[(free_block*4)+j] = FALSE;

					spare_data_list[i].lpn =  -1;
					spare_data_list[i].is_invalid = FALSE;
					isExist[i] = FALSE;
				}
			}

			dd_erase(gb); // garbage 블럭 정리

		}

		for(i = gp, check = 0; i < gp + 4; i++, check++){
			for(j = 0; j < DATAPAGES_PER_DEVICE; j++){
				if(table[j][1] == i)
					table[j][1] = (free_block*4)+check;
			}
		} 
		free_block = gb;
	}

	if(table[lsn][1] > -1) // 업데이트 필요할 경우
	{
		for(i = 0; i < DATAPAGES_PER_DEVICE + 4; i++){ //free page 선정

			if(spare_data_list[i].is_invalid == FALSE && isExist[i] == FALSE) // freepage면 continue
				continue;

			check = TRUE;

			for(j = 0; j < DATAPAGES_PER_DEVICE; j++){ // table에 존재하는 page인지
				if(table[j][1] == i){
					check = FALSE;
					break;
				}
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
		for(i = 0; i < DATAPAGES_PER_DEVICE + 4; i++) //free page 선정
		{ 
			if(spare_data_list[i].is_invalid == FALSE && isExist[i] == FALSE) // freepage면 continue
				continue;

			check = TRUE;

			for(j = 0; j < DATAPAGES_PER_DEVICE; j++){ // table에 존재하는 page인지
				if(table[j][1] == i){
					check = FALSE;
					break;
				}
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
		printf("%3d %3d\n", table[i][0], table[i][1]);

	printf("free block's pbn=%d\n", free_block);

	return;
}
