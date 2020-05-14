#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sectormap.h"

FILE *flashfp;	// fdevicedriver.c에서 사용
int create_file(char *filename, int blocknum);
int exc(char *filename);
int exc1(char *filename);
int exc2(char *filename);
void ftl_open();
void ftl_read(int lsn, char *sectorbuf);
void ftl_write(int lsn, char *sectorbuf);
void ftl_print();

int main(int argc, char **argv){

	if(*argv[1]=='c'){
		if(argc != 4){
			printf("please check\n");
			return -1;	
		}
		create_file(argv[2],atoi(argv[3]));
	}   
	else if(*argv[1]=='s'){
		exc2(argv[2]);
	}
	else{printf("please check option\n");}

	return 0;
}

int exc(char *filename){
	char buf[512];
	char nbuf[512];
	char test[512]="hello, it's me";
	memset((void *)buf,(char)0x41,512);
	buf[511]='\0';
	ftl_open();		
	if((flashfp = fopen(filename,"r+b")) == NULL){
		printf("file open error\n");
		return 1;
	}
	for(int i =0; i<DATAPAGES_PER_DEVICE;i++){
		buf[0]=0x30;
		ftl_write(0,buf);
	}
	ftl_write(0,test);
	ftl_write(1,test);

	ftl_read(0,nbuf);
	printf("### read %d : %s\n",0,nbuf);
	ftl_read(1,nbuf);
	printf("### read %d : %s\n",1,nbuf);
	ftl_print();
	fclose(flashfp);
	return 1;
}
int exc1(char *filename){
	char buf[512];
	char nbuf[512];
	memset((void *)buf,(char)0x41,512);
	buf[511]='\0';
	ftl_open();		
	if((flashfp = fopen(filename,"r+b")) == NULL){
		printf("file open error\n");
		return 1;
	}

	for(int i =0; i<DATAPAGES_PER_DEVICE-1;i++){
		buf[0]=0x41+i;
		ftl_write(i,buf);
	}
	ftl_write(10,buf);
	ftl_write(11,buf);
	ftl_write(11,buf);
	for(int i = 0; i<DATAPAGES_PER_DEVICE;i++){
		ftl_read(i,nbuf);

		printf("### read %d : %s\n",i,nbuf);

	}
	ftl_print();
	fclose(flashfp);
	return 1;
}
int exc2(char *filename){
	char buf[512];
	char nbuf[512];
	char test[512]="hello, it's me";

	memset((void *)buf,(char)0x41,512);
	buf[511]='\0';
	ftl_open();		
	if((flashfp = fopen(filename,"r+b")) == NULL){
		printf("file open error\n");
		return 1;
	}

	for(int i =0; i<1;i++){
		buf[0]=0x41+i;
		ftl_write(0,buf);
	}
	for(int i =0; i<2;i++){
		buf[0]=0x41+i;
		ftl_write(1,buf);
	}
	for(int i =0; i<3;i++){
		buf[0]=0x41+i;
		ftl_write(2,buf);
	}
	for(int i =0; i<4;i++){
		buf[0]=0x41+i;
		ftl_write(3,buf);
	}
	for(int i =0; i<5;i++){
		buf[0]=0x41+i;
		ftl_write(4,buf);
	}
	for(int i =0; i<6;i++){
		buf[0]=0x41+i;
		ftl_write(5,buf);
	}
	for(int i =0; i<7;i++){
		buf[0]=0x41+i;
		ftl_write(6,buf);
	}
	for(int i =0; i<8;i++){
		buf[0]=0x41+i;
		ftl_write(7,buf);
	}
	for(int i =0; i<9;i++){
		buf[0]=0x41+i;
		ftl_write(8,buf);
	}
	for(int i =0; i<10;i++){
		buf[0]=0x41+i;
		ftl_write(9,buf);
	}
	for(int i =0; i<11;i++){
		buf[0]=0x41+i;
		ftl_write(10,buf);
	}
	for(int i =0; i<12;i++){
		buf[0]=0x41+i;
		ftl_write(11,buf);
	}
	ftl_write(10,test);
	for(int i = 0; i<12;i++){
		memset((void*)nbuf,(char)0xFF,PAGE_SIZE);

		ftl_read(i,nbuf);

		printf("### read %d : %s\n",i,nbuf);

	}
	ftl_print();
	fclose(flashfp);
	return 1;
}
int create_file(char *filename,int blocknum){

	char blockbuf[BLOCK_SIZE];
	int ret;
	FILE *fp = fopen(filename,"wb");
	int test;
	if( fp == NULL){
		printf("file open error\n");
		return -1;
	}

	memset((void*)blockbuf,(char)0xFF,BLOCK_SIZE);

	for(int i = 0;i<blocknum;i++){
		ret = fwrite((void *)blockbuf, BLOCK_SIZE, 1, fp);
		if(ret != 1){
			printf("create_file : write error\n");
			return -1;
		}
	}

	fclose(fp);
	return 0;
}


