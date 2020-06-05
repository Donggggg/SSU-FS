package project;

public class Data {
	// charClass 선언
	static int LETTER = 0;
	static int DIGIT = 1;
	static int UNKNOWN = 99;

	// 토큰 문자열 / 숫자 
	static int INT_LIT = 10;
	static int IDENT = 11;

	// 토큰 코드 리스트 [lookup 테이블]
	static int ASSIGN_OP = 20;
	static int ADD_OP = 21;
	static int SUB_OP = 22;
	static int MULT_OP = 23;
	static int DIV_OP = 24;
	static int LEFT_PAREN = 25;
	static int RIGHT_PAREN = 26;
	static int LEFT_INEQU = 27;
	static int RIGHT_INEQU = 28;
	static int LEFT_BRK = 29;
	static int RIGHT_BRK = 30;
	static int EQU_OP = 31;
	static int SEMI_COL = 32;
}
