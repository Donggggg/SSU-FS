############## SunOS ########################
CC       = gcc -g
CCOPTS   = -c #-Wall
LINK     = gcc -g

DEPEND  = makedepend

LINKOPTS = -lm -lnsl

TARGET = a.out
#############################################

SOURCES = person.c

OBJECTS = person.o

.SUFFIXES : .C .c .o

.c.o:
	$(CC) $(CCOPTS) $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINK) $(OBJECTS) $(LINKOPTS)

clean:
	rm *.o person.dat ./a.out

depend : $(SOURCES)
	$(DEPEND) $(DEFINES) -I/usr/local/lib/g++-include $(SOURCES)

# DO NOT DELETE
#############################################

insert:
	./a.out i person.dat 00000000 page1_record0 00 0000 000-0000-0000  0000@gmail.com
	./a.out i person.dat 11111111 page1_record1 11 1111 111-1111-1111  1111@gmail.com
	./a.out i person.dat 22222222 page2_record0 22 2222 222-2222-2222  2222@gmail.com
	./a.out i person.dat 33333333 page2_record1 33 3333 333-3333-3333  3333@gmail.com
	./a.out i person.dat 44444444 page3_record0 44 4444 444-4444-4444  4444@gmail.com
	./a.out i person.dat 55555555 page3_record1 55 5555 555-5555-5555  5555@gmail.com

delete:
	./a.out d person.dat 22222222
	./a.out d person.dat 55555555
	./a.out d person.dat 33333333

reuse:
	./a.out i person.dat 00003333 page2_record1 33 3333 333-3333-3333  3333@gmail.com
	./a.out i person.dat 00005555 page3_record1 55 5555 555-5555-5555  5555@gmail.com
#./a.out i person.dat 00002222 page2_record0 22 2222 222-2222-2222  2222@gmail.com
