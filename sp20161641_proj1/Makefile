CC = gcc
CFLAGS = -Wall
TARGET = 20161641.out
OBJECTS = 20161641.o functions.o InstructionProcessing.o MemoryInstructions.o OpcodeTable.o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^


clean : 
	rm -f *.o
	rm -f $(TARGET)

