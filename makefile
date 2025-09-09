# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -O2

# froth program
OBJ = main.o
TARGET = froth

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

# Files to clean (quote paths for Windows)
CLEAN_OBJS = "main.o"
CLEAN_TARGETS = "$(TARGET)"

.PHONY: all clean

# Build program
all: $(TARGET) $(VM_TARGET)

# froth build
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)
	$(RM) $(CLEAN_OBJS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

# Clean all generated files
clean:
	$(RM) $(CLEAN_OBJS) $(CLEAN_TARGETS) 2>nul
