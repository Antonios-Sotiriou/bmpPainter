# Compile all c files to crwindow executable!

CC = gcc
CFLAGS = -Wall -g
OBJ = crwindow
RM = rm

all:
	$(CC) $(CFLAGS) crwindow.c -o $(OBJ) 
exec:

clean:
