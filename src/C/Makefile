# Compiler to use
CC = gcc

# Flags to pass to the compiler
CFLAGS = -Wall -O2 -g

# Target executable name
TARGET = simulation

# Source files
SRCS = main.c grid.c neuron_encoding.c genetic_operations.c simulation.c gene_encoding.c

# Object files generated from source files
OBJS = $(SRCS:.c=.o)

# Rule to link object files to create target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lm

# Rule to compile source files to object files
.c.o:
	$(CC) $(CFLAGS) -c $<

# Rule for cleaning up object files and target executable
clean:
	rm -f $(OBJS) $(TARGET)
