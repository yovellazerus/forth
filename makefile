# Compiler settings
CC = gcc
CFLAGS = # -Wall -O2

# cfroth program
OBJ = main.o utils.o Lexer.o
TARGET = cfroth

# VM program
VM_OBJ = vm/vmMain.o vm/instruction.o vm/vm.o
VM_TARGET = vm\machine

# Default remove command
RM = rm -f
EXE_EXT =

# Detect Windows
ifeq ($(OS),Windows_NT)
    RM = del /Q
    EXE_EXT = .exe
endif

# Add executable extensions
TARGET := $(TARGET)$(EXE_EXT)
VM_TARGET := $(VM_TARGET)$(EXE_EXT)

# Files to clean (quote paths for Windows)
CLEAN_OBJS = "main.o" "utils.o" "Lexer.o"
CLEAN_TARGETS = "$(TARGET)"
CLEAN_VM_OBJS = "vm\vmMain.o" "vm\instruction.o" "vm\vm.o"
CLEAN_VM_TARGETS = "$(VM_TARGET)"

.PHONY: all clean

# Build both programs
all: $(TARGET) $(VM_TARGET)

# cfroth build
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)
	$(RM) $(CLEAN_OBJS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

Lexer.o: Lexer.c
	$(CC) $(CFLAGS) -c Lexer.c

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c

# VM build
$(VM_TARGET): $(VM_OBJ)
	$(CC) $(CFLAGS) -o $(VM_TARGET) $(VM_OBJ)
	$(RM) $(CLEAN_VM_OBJS)

vm/vmMain.o: vm/vmMain.c vm/instruction.h vm/vm.h
	$(CC) $(CFLAGS) -c vm/vmMain.c -o vm/vmMain.o

vm/instruction.o: vm/instruction.c vm/instruction.h
	$(CC) $(CFLAGS) -c vm/instruction.c -o vm/instruction.o

vm/vm.o: vm/vm.c vm/vm.h
	$(CC) $(CFLAGS) -c vm/vm.c -o vm/vm.o

# Clean all generated files
clean:
	$(RM) $(CLEAN_OBJS) $(CLEAN_TARGETS) 2>nul
	$(RM) $(CLEAN_VM_OBJS) $(CLEAN_VM_TARGETS) 2>nul
