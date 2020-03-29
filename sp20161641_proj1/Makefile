CC = gcc
CFLAGS = -Wall
TARGET = test
OBJECTS = 20161641.o functions.o InstructionProcessing.o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^


clean : 
	rm -f *.o
	rm -f $(TARGET)

