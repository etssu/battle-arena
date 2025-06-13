# Compiler
CC = gcc

# Compilation flags
CFLAGS = -Wall -g

# Linking flags
LDFLAGS = -lncurses

# Source files
SRC = main.c graphics.c logic.c data.c

# Object files (.o)
OBJ = $(SRC:.c=.o)

# Name of the executable program
EXEC = z6

# Rule to build the executable file
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Rule to compile .c files into .o files
.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)

# Rule to clean up compiled files
clean:
	rm -f $(OBJ) $(EXEC)

# Rule to rebuild the project (clean and then build)
rebuild: clean $(EXEC)
