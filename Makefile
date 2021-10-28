# Compile all c files to crwindow executable!

CC = gcc
CFLAGS = -Wall -g
OBJ = crwindow
RM = rm
LINKS = -lX11

all:
	$(CC) $(CFLAGS) crwindow.c -o $(OBJ) $(LINKS)
exec:

clean:
