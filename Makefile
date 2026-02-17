CC=gcc
CFLAGS=-g -Wall
AR=ar
ARFLAGS=-cvrs

LIB=libmythreads.a
LIBSRC=threadmanagement.c lockmanagement.c
LIBOBJ=$(LIBSRC:.c=.o)

DIST=project2.tgz
FILES=Makefile README $(LIBSRC) mythreads.h


all: $(LIB)

$(LIB): $(LIBOBJ)
	$(RM) $(LIB)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

tar: $(FILES)
	tar cvzf $(DIST) $(FILES)

clean:
	$(RM) $(BINS) $(LIB) *.o
	$(RM) -r *.dSYM
