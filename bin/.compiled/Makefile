CC=cc
CFLAGS=-O3

ifeq ($(shell uname -s),OpenBSD)
CFLAGS+="-I/usr/X11R6/include" 
LDFLAGS+="-L/usr/X11R6/lib" 
endif


all: getfocusedwindowname


getfocusedwindowname:
	@echo ${CC} ${CFLAGS} ${LDFLAGS} -lX11 getfocusedwindowname.c -o getfocusedwindowname
	@${CC} ${CFLAGS} ${LDFLAGS} -lX11 getfocusedwindowname.c -o getfocusedwindowname

clean:
	@rm -f getfocusedwindowname

install: all
	@install -b -m 755 -s getfocusedwindowname ../getfocusedwindowname
