package project;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;

public class Main {
	public static void main(String[] args) {  
		File file; // 소스 코드 파일 
		FileReader file_reader; // 소스 코드 파일리더
		Analyzer analyzer; // 소스 분석을 할 Analyzer 객체 

		try{
			file = new File("front.in"); // 소스코드 오픈
			file_reader = new FileReader(file); // 파일리더 객체 생성
			analyzer = new Analyzer(file_reader); // Analyzer 객체 생성
			analyzer.getChar(); // 소스 코드 첫 데이터 분석..
			do{
				analyzer.lex(); // 토큰화 작업 
			}while(analyzer.nextToken != -1); // EOF를 만날 때까지 분석 반복..
		}
		catch(IOException e){ // 예외 처리
			e.getStackTrace();
		}     
	}
}
