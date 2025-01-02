CC = clang
CFLAGS = -Wall -Wextra -Werror -Iinclude -I/opt/homebrew/include/
LDFLAGS = -L/opt/homebrew/lib -lcmocka

TARGET = test

all: $(TARGET)

compile_commands:
	bear -- make

$(TARGET): test.o
	$(CC) $(LDFLAGS) -o $(TARGET) test.o

test.o: test.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o ./test

.PHONY: clean
