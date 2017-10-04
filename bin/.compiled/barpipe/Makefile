XLIBS=-lm -lxcb -lxcb-icccm -lxcb-ewmh
LDFLAGS=-lconfuse -L/usr/local/lib
CFLAGS= -g -W -Wall -O2 -I/usr/local/include

ifeq ($(shell uname), Linux)
# Linux needs libbsd 
LDFLAGS         += $(shell pkg-config --libs libbsd)
CFLAGS          += $(shell pkg-config --cflags libbsd)
endif

LDFLAGS+=$(XLIBS)

TARGET = barpipe
OBJS = main.o config.o funcs.o title.o

all: $(TARGET)

install: all
	@install -m 0555 $(TARGET) $$HOME/bin/$(TARGET)
clean:
	@rm -f $(TARGET) $(OBJS)

$(TARGET): $(OBJS)
	$(CC) $(STATIC) -o $@ $(OBJS) $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@


