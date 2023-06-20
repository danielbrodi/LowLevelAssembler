CC = gcc
CFLAGS = -ansi -pedantic -Wall

SRC = gpt_daniel.c macro.c utils.c binary.c create_ob.c

TARGET = assembler

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^  -lm -o $@

clean:
	rm -f $(TARGET)
