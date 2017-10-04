CC=cc
CFLAGS=-O3

all: getfocusedwindowname


getfocusedwindowname:
	@echo ${CC} ${CFLAGS} -lX11 getfocusedwindowname.c -o getfocusedwindowname
	@${CC} ${CFLAGS} -lX11 getfocusedwindowname.c -o getfocusedwindowname

clean:
	@rm -f getfocusedwindowname

install: all
	@install -b -m 755 -s getfocusedwindowname ../getfocusedwindowname
