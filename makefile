CC = gcc
CFLAGS = -Wall -O2
OBJ = main.o Lexer.o
TARGET = program
RM = rm -f

ifeq ($(OS),Windows_NT)
    RM = del /Q
    TARGET = program.exe
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)
	$(RM) $(OBJ)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

Lexer.o: Lexer.c
	$(CC) $(CFLAGS) -c Lexer.c

clean:
	$(RM) $(OBJ) $(TARGET)
