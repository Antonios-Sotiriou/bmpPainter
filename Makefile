# Compile all c files to crwindow executable!

CC = gcc
CFLAGS = -Wall -Werror -g
OBJ = crwindow
RM = rm
LINKS = -lX11

install:
	./install.sh;

uninstall:
	sudo apt remove libx11-dev;

all:
	$(CC) $(CFLAGS) crwindow.c locale.c bmp_parser.c -o $(OBJ) $(LINKS);

exec:
	./crwindow;

clean:
	sudo rm -r ./*;
	sudo apt autoremove -y;
