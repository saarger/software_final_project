CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O2
TARGET = symnmf
SRC = symnmf.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)