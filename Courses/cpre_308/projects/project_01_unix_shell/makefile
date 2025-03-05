# Makefile for UNIX Shell Project 1
# Michael Becker
# CPRE 308

# Need to make the compiler and the flags
CC = gcc #using gcc to compile
CFLAGS = -Wall -Wextra -g #wall and wextra enable warnings and -g adds debug info

# Defines the target which in this case is shell
TARGET = shell

# Need to define the source and object files and creates an object file of shell as well
SRCS = shell.c
OBJS = $(SRCS:.c=.o)

# This is the default target
all: $(TARGET)

# Need to take the object files and compiles them to be able to execute
# The dependendencies are the object file
#the second line is the command to run
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) 
	

# Compiling the source files to object files.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Need to clean up the files that have been compiled
clean:
	rm -f $(TARGET) $(OBJS)


# Run with custom prompt
custom: $(TARGET)
	./$(TARGET) -p "custom> "


