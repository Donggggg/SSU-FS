CC     = gcc #-g
CCOPTS = -c
LINK   = gcc #-g

SEQUENCE = read_seq
RANDOM   = read_rand
ALL      = $(SEQUENCE) $(RANDOM)

FILE1  = students1.dat
FILE2  = students2.dat

#################################################

.SUFFIXES : .C .c .o

.c.o : 
	$(CC) $(CCOPTS) $<

$(SEQUENCE) : read_seq.o
	$(LINK) -o $(SEQUENCE) read_seq.o

$(RANDOM) : read_rand.o
	$(LINK) -o $(RANDOM) read_rand.o

run_seq :
	./$(SEQUENCE) $(FILE1) > seq_output.txt

run_rand :
	./$(RANDOM)   $(FILE2) > rand_output.txt

clean :
	rm *.o
	rm *.dat
	rm $(ALL) 
	rm *.txt
