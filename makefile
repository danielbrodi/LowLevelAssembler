CC = gcc
CFLAGS = -ansi -pedantic -Wall

SRC = gpt.c macro.c utils.c

TARGET = mycomp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^  -lm -o $@

clean:
	rm -f $(TARGET)
