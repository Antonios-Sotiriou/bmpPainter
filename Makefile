# Compile all c files to crwindow executable!

CC = gcc
CFLAGS = -Wall -Werror -g
OBJ = crwindow
RM = rm
LINKS = -lX11

all:
	$(CC) $(CFLAGS) crwindow.c locale.c bmp_parser.c -o $(OBJ) $(LINKS)
exec:

clean:
