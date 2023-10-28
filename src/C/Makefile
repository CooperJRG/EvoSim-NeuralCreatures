# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -Wall -O2

# Source files
SRCS = main.c neuron_encoding.c gene_encoding.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
EXEC = neural_network_test

all: $(EXEC)

# Linking the executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) -lm

# Compiling source files into object files
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# Cleaning up object files and executable
clean:
	rm -f $(OBJS) $(EXEC)
