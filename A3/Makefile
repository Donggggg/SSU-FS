a.out : fdevicedriver.o ftlmgr.o
	gcc fdevicedriver.o ftlmgr.o -o a.out

fdevicedriver.o : fdevicedriver.c flash.h
	gcc -c fdevicedriver.c

ftlmgr.o : ftlmgr.c flash.h
	gcc -c ftlmgr.c

clean : 
	rm *.o
	rm a.out
