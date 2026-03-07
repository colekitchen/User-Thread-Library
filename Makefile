CC=gcc
CFLAGS=-g -Wall
AR=ar
ARFLAGS=-cvrs

LIB=libmythreads.a
LIBSRC=threadmanagement.c
LIBOBJ=$(LIBSRC:.c=.o)

DIST=project2.tgz
FILES=Makefile README $(LIBSRC) mythreads.h


all: $(LIB)

$(LIB): $(LIBOBJ)
	$(RM) $(LIB)
	$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

testc: cooperative_test.c libmythreads.a
	$(CC) -o testc cooperative_test.c libmythreads.a

testp: preemptive_test.c libmythreads.a
	$(CC) -o testp preemptive_test.c libmythreads.a

testl: lock_test.c libmythreads.a
	$(CC) -o testl lock_test.c libmythreads.a 

testm: mem_test.c libmythreads.a
	$(CC) -o testm mem_test.c libmythreads.a

testcv: cv_test.c libmythreads.a
	$(CC) -o testcv cv_test.c libmythreads.a
	
tar: $(FILES)
	tar cvzf $(DIST) $(FILES)

clean:
	$(RM) $(BINS) $(LIB) *.o
	$(RM) -r *.dSYM
