# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror # -std=c11 

# Source files
SRCS = ./src/main.c ./src/midi.c ./src/fcb.c ./src/ui_ncurses.c ./src/fcb_io.c

# Object files
OBJS = $(SRCS:./src/%.c=./build/obj/%.o)

# Output executable
TARGET = ./build/bin/fcbtool

# Default target
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lasound -lncurses

# Rule to compile source files into object files
./build/obj/%.o: ./src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Run target
run: $(TARGET)
	./$(TARGET)

debug: clean all run

.PHONY: all clean

