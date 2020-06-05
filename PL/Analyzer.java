package project;

import java.io.FileReader;
import java.io.IOException;

public class Analyzer {
	int nextChar; // 토큰의 1바이트
	int charClass; // charClass 변수[LETTER, DIGIT, UNKNOWN, EOF]
	int nextToken; // 토큰의 종류 
	int lexLen; // 토큰의 길이
	char lexeme[] = new char[100]; // 토큰
	FileReader file_reader; // 소스코드 파일 리더 

	Analyzer(FileReader file_reader){ // 생성자 
		this.file_reader = file_reader;
	}

	void addChar() // 토큰을 완성시키는 메소드
	{
		if(lexLen < 100){ // 최대길이를 초과하지 않으면
			lexeme[lexLen++] = (char)nextChar; //토큰에 1바이트 이어붙임 
			for(int i = lexLen; i < 100; i++) // 의미 없는 공간을 널로 세팅
				lexeme[i] = '\0';
		}
		else // 예외 처리
			System.out.println("Error - lexeme is too long");
	}

	void getChar() // 토큰을 구분하는 메소드
	{
		try{
			if((nextChar = file_reader.read()) != -1){ // 파일에서 1바이트 입력 받음
				if(Character.isLetter(nextChar)) // 알파벳(대, 소문자)인 경우 
					charClass = Data.LETTER;
				else if(Character.isDigit(nextChar)) // 숫자(0~9)인 경우
					charClass = Data.DIGIT;
				else // 그 외의 경우
					charClass = Data.UNKNOWN;
			}
			else // 파일의 끝이면 
				charClass = -1; // EOF 세팅
		}
		catch(IOException e){ // 예외처리
			e.getStackTrace();
		}
	}

	void getNonBlank()
	{
		while(Character.isWhitespace(nextChar)) // 공백 문자가 아닐 때까지 파일 탐색
			getChar(); 
	}

	void lex()
	{
		lexLen = 0; // 토큰 길이 0으로 세팅
		getNonBlank(); // 공백문자 스킵

		switch(charClass){

			case 0 : // LETTER
				addChar();
				getChar();
				while(charClass == Data.LETTER || charClass == Data.DIGIT){
					addChar();
					getChar();
				}
				nextToken = Data.IDENT;
				break;
			case 1 : // DIGIT
				addChar();
				getChar();
				while(charClass == Data.DIGIT){
					addChar();
					getChar();
				}
				nextToken = Data.INT_LIT;
				break;                    
			case 99 : // UNKNOWN
				lookup((char)nextChar); // lookup 테이블 상에 존재하는 1바이트 기호인지 검사
				getChar(); // 다음 문자 세팅
				break;
			case -1 : // EOF 처리
				nextToken = -1;
				lexeme[0] = 'E';
				lexeme[1] = 'O';
				lexeme[2] = 'F';
				for(i = 3; i < 100; i++) // 의미 없는 공간 널문자 
					lexeme[i] = '\0';
				break;
		}

		String s = String.valueOf(lexeme); // char[] to String
		System.out.println("Next Token is: " + nextToken + ", Next Lexeme is "+s);
	}

	int lookup(char ch) 
	{
		/* 해당토큰의 종류를 구분해 토큰(lexeme)에 저장 후 
		   토큰 종류를 세팅한다. */

		switch (ch) { // 토큰에 따라 분류

			case '(':
				addChar();
				nextToken = Data.LEFT_PAREN;
				break;
			case ')':
				addChar();
				nextToken = Data.RIGHT_PAREN;
				break;
			case '{':
				addChar();
				nextToken = Data.LEFT_BRK;
				break;
			case '}':
				addChar();
				nextToken = Data.RIGHT_BRK;
				break;
			case '+':
				addChar();
				nextToken = Data.ADD_OP;
				break;
			case '-':
				addChar();
				nextToken = Data.SUB_OP;
				break;
			case '*':
				addChar();
				nextToken = Data.MULT_OP;
				break;
			case '/':
				addChar();
				nextToken = Data.DIV_OP;
				break;
			case '=':
				addChar();
				nextToken = Data.EQU_OP;
				break;
			case '>':
				addChar();
				nextToken = Data.LEFT_INEQU;
				break;
			case '<':
				addChar();
				nextToken = Data.RIGHT_INEQU;
				break;
			case ';':
				addChar();
				nextToken = Data.SEMI_COL;
				break;
			default : // lookup 테이블에 없는 경우 EOF로 인지
				addChar();
				nextToken = -1;
				break;

		}
		return nextToken; // 토큰 종류 리턴
	}
}
