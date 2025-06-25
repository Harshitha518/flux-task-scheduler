# Define compiler
CC=gcc
# Define flags
CFLAGS=-Wall -Wextra -std=c11

all: flux

# .c files that are being compiled to object files
flux: main.o task.o
	$(CC) $(CFLAGS) -o flux main.o task.o

# For each .o compile the matching .c file
main.o: main.c task.h
	$(CC) $(CFLAGS) -c main.c

task.o: task.c task.h
	$(CC) $(CFLAGS) -c task.c

# Cleanup rule (make clean --> to delete all compiled files)
clean:
	rm -f *.o flux
