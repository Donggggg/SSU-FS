#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "sectormap.h"

int dd_erase(int pbn);
void ftl_open();
void ftl_print();
void ftl_write(int lsn, char *sectorbuf);
void ftl_read(int lsn, char *sectorbuf);

FILE *flashfp;
int main()
{
	int i, md;
	int lsn;
	char string[512];
	char *sector = malloc(sizeof(char) * SECTOR_SIZE);

//	if(access("flashfile", F_OK) < 0)
		flashfp = fopen("flashfile", "w+");
//	else
//		flashfp = fopen("flashfile", "r+");

	for(i = 0; i < BLOCKS_PER_DEVICE; i++)
		dd_erase(i);

	ftl_open();


	while(1){
		printf("Input : ");
		scanf("%d", &md);

		switch(md) {
			case 1 : // open
				break;
			case 2 : // write
				printf("lsn / sector : ");
				scanf("%d %s", &lsn, string);
				ftl_write(lsn, string);
				break;
			case 3:  // read
				printf("lsn : ");
				scanf("%d", &lsn);
				ftl_read(lsn, sector);
				printf("%s\n", sector);
				break;
			case 4: // print
				ftl_print();
				break;
		}
	}
	return 0;

}
