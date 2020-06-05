package project;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;

public class Main {
	public static void main(String[] args) {  
		File file;
		FileReader file_reader;
		Analyzer analyzer;

		try{
			file = new File("/workspace/java_test/src/project/front.in");
			file_reader = new FileReader(file);
			analyzer = new Analyzer(file_reader);
			analyzer.getChar();
			do{
				analyzer.lex();
			}while(analyzer.nextToken != -1);
		}
		catch(IOException e){
			e.getStackTrace();
		}     
	}
}
