#########################################
CC       = gcc
CCOPTS   = -c
LINK     = gcc
LINKOPTS = -lm 

TARGET       = ftlmgr
FLASHMEMORY  = flashmemory

#########################################
.SUFFIXES : .C.c.o

SOURCES = ftlmgr.c fdevicedriver.c

OBJECTS = ftlmgr.o fdevicedriver.o

.c.o : 
	$(CC) $(CCOPTS) $<

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(LINK) -o $(TARGET) $(OBJECTS) $(LINKOPTS)

clean :
	rm *.o ./$(TARGET) ./$(FLASHMEMORY)

##########################################
create :
	./$(TARGET) c $(FLASHMEMORY) 5

write :
	./$(TARGET) w $(FLASHMEMORY) 0 "A00" "0"
	./$(TARGET) w $(FLASHMEMORY) 4 "B00" "4"
	./$(TARGET) w $(FLASHMEMORY) 5 "B01" "5"
	./$(TARGET) w $(FLASHMEMORY) 7 "B03" "7"

update:
	./$(TARGET) w $(FLASHMEMORY) 7 "U03" "7"
	
read :
	./$(TARGET) r $(FLASHMEMORY) 0 > output0.txt
	./$(TARGET) r $(FLASHMEMORY) 7 > output7.txt

erase :
	./$(TARGET) e $(FLASHMEMORY) 1
