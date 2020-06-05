#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// 함수 원형
void addChar();
void getChar();
void getNonBlank();
int lex();

// charClass 선언
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99

// 토큰 문자열 / 숫자 
#define INT_LIT 10
#define IDENT 11
// 토큰 코드 리스트 [lookup 테이블]
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26
#define LEFT_INEQU 27
#define RIGHT_INEQU 28
#define LEFT_BRK 29
#define RIGHT_BRK 30
#define EQU_OP 31
#define SEMI_COL 32

int charClass; // charClass 변수[LETTER, DIGIT, UNKNOWN, EOF]
int lexLen; // 토큰 길이 
int nextToken; // 토큰의 종류 
char lexeme[100]; // 토큰 
char nextChar; // 토큰의 1바이트
FILE *in_fp; // 분석할 코드 파일

int main() 
{
	if((in_fp = fopen("front.in", "r")) == NULL) // 읽기 전용으로 소스코드 파일 open
		printf("ERROR - cannot open front.in \n");
	else{
		getChar(); // 코드파일의 시작 바이트 처리 후
		do{
			lex(); // 토크나이징 작업..
		}while(nextToken != EOF); // EOF에 도달할 때까지 반복
	}
}

int lookup(char ch) 
{
	/* 해당토큰의 종류를 구분해 토큰(lexeme)에 저장 후 
	   토큰 종류를 세팅한다. */

	switch (ch) { // 토큰에 따라 분류

		case '(':
			addChar();
			nextToken = LEFT_PAREN;
			break;
		case ')':
			addChar();
			nextToken = RIGHT_PAREN;
			break;
		case '{':
			addChar();
			nextToken = LEFT_BRK;
			break;
		case '}':
			addChar();
			nextToken = RIGHT_BRK;
			break;
		case '+':
			addChar();
			nextToken = ADD_OP;
			break;
		case '-':
			addChar();
			nextToken = SUB_OP;
			break;
		case '*':
			addChar();
			nextToken = MULT_OP;
			break;
		case '/':
			addChar();
			nextToken = DIV_OP;
			break;
		case '=':
			addChar();
			nextToken = EQU_OP;
			break;
		case '>':
			addChar();
			nextToken = LEFT_INEQU;
			break;
		case '<':
			addChar();
			nextToken = RIGHT_INEQU;
			break;
		case ';':
			addChar();
			nextToken = SEMI_COL;
			break;
		default : // lookup 테이블에 없는 경우 EOF로 인지
			addChar();
			nextToken = EOF;
			break;

	}
	return nextToken; // 토큰 종류 리턴
}

void addChar()  // 토큰을 완성시키는 함수
{
	if(lexLen <= 99){ // 최대길이 보다 작은 경우
		lexeme[lexLen++] = nextChar; // 토큰에 1바이트씩 이어붙임
		lexeme[lexLen] = 0; // 마지막문자를 NULL로 세팅
	}
	else // 에러 처리
		printf("Error - lexeme is too long \n");
}

void getChar() // 토큰 구분하는 함수
{
	/** 한 바이트만 검사하여 토큰의 class를 구분한다. **/

	if((nextChar = getc(in_fp)) != EOF){ // 파일에서 1바이트 입력받음
		if(isalpha(nextChar)) // 알파벳(대,소문자)인 경우
			charClass = LETTER;
		else if(isdigit(nextChar)) // 숫자(0~9)인 경우 
			charClass = DIGIT;
		else // 그 외의 경우 
			charClass = UNKNOWN;
	}
	else // 파일의 끝이면 EOF
		charClass = EOF;
}

void getNonBlank() // 공백문자 제거해주는 함수
{
	while(isspace(nextChar)) // nextChar가 공백 문자가 아닐 때까지 파일 탐색 
		getChar();
}

int lex()
{
	lexLen = 0; // 토큰길이 0으로 세팅
	getNonBlank(); // 공백문자 스킵

	//*************************************** 토큰 생성법 ****************************************//
	// LETTER > 첫 문자가 LETTER이면 그 다음 부턴 LETTER나 DIGIT이면 같은 토큰으로 인식
	// DIGIT > 첫 문자가 DIGIT이면 DIGIT이 아닐 때까지 같은 토큰으로 인식
	// UNKNOWN > lookup함수를 통해 lookup테이블을 참고하여 토큰 생성
	// EOF > 즉시 EOF를 토큰으로 생성

	switch (charClass){ 

		case LETTER : // LETTER인 경우
			addChar(); 
			getChar();
			while (charClass == LETTER || charClass == DIGIT) { // LETTER나 DIGIT이 아닐때 까지 이어 붙여서 토큰 완성시켜 나감
				addChar();
				getChar();
			}
			nextToken = IDENT; // 토큰 종류 명시 (문자열)
			break;
		case DIGIT :
			addChar();
			getChar();
			while(charClass == DIGIT){ // DIGIT이 아닐때 까지 이어 붙여서 토큰 완성시켜 나감
				addChar();
				getChar();
			}
			nextToken = INT_LIT; // 토큰 종류 명시 (숫자)
			break;
		case UNKNOWN :
			lookup(nextChar); // lookup 테이블 상에 존재하는 1바이트 기호인지 검사
			getChar(); // 다음 문자 세팅
			break;
		case EOF : // EOF 처리
			nextToken = EOF;
			lexeme[0] = 'E';
			lexeme[1] = 'O';
			lexeme[2] = 'F';
			lexeme[3] = 0;
			break;
	}

	printf("Next Token is: %d, Next Lexeme is %s\n",
			nextToken, lexeme);

	return nextToken;
}
