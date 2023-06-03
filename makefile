CC = gcc
CFLAGS = -ansi -pedantic -Wall

SRC = gpt.c

TARGET = mycomp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^  -lm -o $@

clean:
	rm -f $(TARGET)
