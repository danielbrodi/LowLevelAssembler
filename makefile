CC = gcc
CFLAGS = -ansi -pedantic -Wall

SRC = gpt_daniel.c macro.c utils.c binary.c

TARGET = mycomp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^  -lm -o $@
	
binary: binary.o
	gcc -g -Wall -ansi -pedantic binary.o -o binary

binary.o: binary.c
	gcc -c -Wall -ansi -pedantic binary.c -o binary.o

clean:
	rm -f $(TARGET)
