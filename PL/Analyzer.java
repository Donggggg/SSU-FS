package project;

import java.io.FileReader;
import java.io.IOException;

public class Analyzer {
	int nextChar;
	int charClass;
	int nextToken;
	int lexLen;
	char lexeme[] = new char[100];
	FileReader file_reader;

	Analyzer(FileReader file_reader){
		this.file_reader = file_reader;
	}

	void addChar()
	{
		if(lexLen < 100){
			lexeme[lexLen++] = (char)nextChar; 
			for(int i = lexLen; i < 100; i++)
				lexeme[i] = '\0';
		}
		else
			System.out.println("Error - lexeme is too long");
	}

	void getChar()
	{
		try{
			if((nextChar = file_reader.read()) != -1){
				if(Character.isLetter(nextChar))
					charClass = Data.LETTER;
				else if(Character.isDigit(nextChar))
					charClass = Data.DIGIT;
				else
					charClass = Data.UNKNOWN;
			}
			else
				charClass = -1; // EOF
		}
		catch(IOException e){
			e.getStackTrace();
		}
	}

	void getNonBlank()
	{
		while(Character.isWhitespace(nextChar))
			getChar(); 
	}

	void lex()
	{
		lexLen = 0;
		getNonBlank();

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
			case 99 :
				lookup((char)nextChar); // lookup 테이블 상에 존재하는 1바이트 기호인지 검사
				getChar(); // 다음 문자 세팅
				break;
			case -1 : // EOF 처리
				nextToken = -1;
				lexeme[0] = 'E';
				lexeme[1] = 'O';
				lexeme[2] = 'F';
				lexeme[3] = '\0';
				break;
		}

		String s = String.valueOf(lexeme);
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
