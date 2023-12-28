CC = gcc
CFLAGS = -ansi -Wall -Wextra -pedantic-errors  -O2
LDFLAGS = -lm
TARGET = symnmf
SRC = symnmf.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)