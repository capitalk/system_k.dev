CC=gcc

CFLAGS=-Wall -pthread -O3 -DLOG
LDFLAGS=-static

ifeq ($(dbg),1)
        CFLAGS = -Wall -pthread -DDEBUG -ggdb
endif

all: libcsoft.a

%.o : %.c
	$(CC) -c $(CFLAGS) $<

libcsoft.a: $(OBJS) 
	ar rcs $@ $+

clean :
	rm -f *.o libcsoft.a

