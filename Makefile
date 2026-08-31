CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -Iinclude -O2
SRC = src/ring_buffer.c tests/test_main.c
TARGET = test_runner

all: $(TARGET)
	./$(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) *.o *.exe
