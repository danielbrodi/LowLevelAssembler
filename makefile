CC = gcc
CFLAGS = -ansi -pedantic -Wall

SRC = assembler.c macro.c vector.c macro_processing.c label_processing.c  binary_and_ob_processing.c am_file_processing.c print_error.c param_validation.c utils.c


TARGET = assembler

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^  -lm -o $@

clean:
	rm -f $(TARGET)

