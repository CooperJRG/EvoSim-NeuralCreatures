# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Source files
SRC = main.c neuron_encoding.c gene_encoding.c

# Object files (one for each source file)
OBJ = $(SRC:.c=.o)

# Name of the executable
EXEC = neural_network

# Default target
all: $(EXEC)

# Linking
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Phony target to clean object files and executable
.PHONY: clean
clean:
	rm -f $(OBJ) $(EXEC)
