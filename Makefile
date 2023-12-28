CC = gcc
CFLAGS = -ansi -Wall -Wextra -pedantic-errors  -O2 -lm
TARGET = symnmf
SRC = symnmf.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)